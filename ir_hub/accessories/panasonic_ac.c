#include "espressif/esp_common.h"

#include "transmit.h"
#include "panasonic_ac.h"
#include "../utilities/error_handler.h"
#include "../utilities/homekit_utility.h"

/* estimated value */
#define CARRIER_FREQUENCY 38000
#define HEADER_MARK 3500
#define HEADER_SPACE 3500
#define BIT_MARK 870
#define ZERO_SPACE 870
#define ONE_SPACE 2600
#define FOOTER_SPACE 13900

void panasonic_ac_identify(homekit_value_t _value);
void panasonic_ac_heating_cooling_state_callback(homekit_characteristic_t *characteristic, homekit_value_t value, void *context);
void panasonic_ac_target_temperature_callback(homekit_characteristic_t *characteristic, homekit_value_t value, void *context);

uint32_t panasonic_ac_encode(homekit_accessory_t* accessory, bool toggle_power);

void panasonic_ac_transmit(uint32_t code);
void panasonic_ac_transmit_16(uint16_t code);

void panasonic_ac_init(homekit_accessory_t* accessory) {
    accessory->category = homekit_accessory_category_thermostat;
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION));
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(THERMOSTAT, .primary=true));

    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, panasonic_ac_identify));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Ob"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MODEL, "ObAC1"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"));

    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_HEATING_COOLING_STATE, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_HEATING_COOLING_STATE, 0, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(panasonic_ac_heating_cooling_state_callback)));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 25));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_TEMPERATURE, 25, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(panasonic_ac_target_temperature_callback)));
    *(accessory->services[1]->characteristics[3]->min_value) = 16.0f;
    *(accessory->services[1]->characteristics[3]->max_value) = 30.0f;
    *(accessory->services[1]->characteristics[3]->min_step) = 1.0f;
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TEMPERATURE_DISPLAY_UNITS, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"));
}

void panasonic_ac_heating_cooling_state_callback(homekit_characteristic_t *characteristic, homekit_value_t value, void *context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    uint8_t state = value.int_value;
    homekit_characteristic_t* current_state_characteristic = characteristic->service->characteristics[0];
    uint8_t current_state = current_state_characteristic->value.int_value;
    bool toggle_power = (state == 0 && current_state != 0) || (current_state == 0 && state != 0);
    panasonic_ac_transmit(panasonic_ac_encode(accessory, toggle_power));
    current_state_characteristic->value = HOMEKIT_UINT8(state);
    homekit_characteristic_notify(current_state_characteristic, current_state_characteristic->value);
}
void panasonic_ac_target_temperature_callback(homekit_characteristic_t *characteristic, homekit_value_t value, void *context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    homekit_characteristic_t* current_temperature_characteristic = characteristic->service->characteristics[2];
    panasonic_ac_transmit(panasonic_ac_encode(accessory, false));
    current_temperature_characteristic->value = HOMEKIT_FLOAT(value.float_value);
    homekit_characteristic_notify(current_temperature_characteristic, current_temperature_characteristic->value);
}

uint32_t panasonic_ac_encode(homekit_accessory_t* accessory, bool toggle_power) {
    uint32_t code = 0x0F002F6C;
    uint8_t state = accessory->services[1]->characteristics[1]->value.int_value;
    uint8_t state_code = 0;
    uint8_t temperature_code = 0;
    if(state == 0) state = accessory->services[1]->characteristics[0]->value.int_value; // when changed to off, read current state
    if(state == 0 || state == 3) { // off or auto
        state_code = 3;
        temperature_code = 8;
    } else {
        uint8_t temperature = (int)accessory->services[1]->characteristics[3]->value.float_value;
        temperature -= 15;
        for(uint8_t i = 0; i < 4; i++) { // reverse temperature code
            temperature_code <<= 1;
            temperature_code += temperature % 2;
            temperature >>= 1;
        }
        if(state == 1) { // heat
            state_code = 1;
        } else { // cool
            state_code = 2;
        }
    }
    code += (uint32_t)state_code << 21;
    code += (uint32_t)temperature_code << 28;
    if(!toggle_power) code += 1 << 20;
    return code;
}

void panasonic_ac_transmit(uint32_t code) {
    transmit_set_carrier(CARRIER_FREQUENCY);
    uint16_t high = code >> 16, low = code;
    panasonic_ac_transmit_16(high);
    panasonic_ac_transmit_16(low);
}
void panasonic_ac_transmit_16(uint16_t code) {
    uint8_t high = code >> 8, low = code;
    transmit_clear_time();
    transmit_mark(HEADER_MARK);
    transmit_space(HEADER_SPACE);
    for(uint8_t i = 0; i < 2; i++) {
        transmit_code(high, 8, BIT_MARK, ZERO_SPACE, ONE_SPACE);
        transmit_code(high, 8, BIT_MARK, ZERO_SPACE, ONE_SPACE);
        transmit_code(low, 8, BIT_MARK, ZERO_SPACE, ONE_SPACE);
        transmit_code(low, 8, BIT_MARK, ZERO_SPACE, ONE_SPACE);
        transmit_mark(HEADER_MARK);
        transmit_space(HEADER_SPACE);
    }
    transmit_mark(BIT_MARK);
    transmit_space(FOOTER_SPACE);
}

void panasonic_ac_identify(homekit_value_t _value) {
    printf("Panasonic AC identify\n");
}