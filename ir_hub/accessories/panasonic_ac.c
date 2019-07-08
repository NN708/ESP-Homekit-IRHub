#include "espressif/esp_common.h"

#include "transmit.h"
#include "panasonic_ac.h"
#include "thermostat.h"
#include "../utilities/error_handler.h"

void panasonic_ac_heating_cooling_state_callback(homekit_characteristic_t *characteristic, homekit_value_t value, void *context);
void panasonic_ac_target_temperature_callback(homekit_characteristic_t *characteristic, homekit_value_t value, void *context);

uint32_t panasonic_ac_encode(homekit_accessory_t* accessory, bool toggle_power);

void panasonic_ac_transmit(uint32_t code);
void panasonic_ac_transmit_16(uint16_t code);

void panasonic_ac_init(homekit_accessory_t* accessory) {
    thermostat_init(accessory);
    
    homekit_service_t* service;
    homekit_characteristic_t** characteristics;

    service = accessory->services[0];
    characteristics = service->characteristics;
    characteristics[1] = NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Ob");
    characteristics[2] = NEW_HOMEKIT_CHARACTERISTIC(MODEL, "ObAC1");
    characteristics[3] = NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat");
    characteristics[4] = NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01");
    characteristics[5] = NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1");
    characteristics[6] = NULL;
    
    service = accessory->services[1];
    characteristics = malloc(sizeof(homekit_characteristic_t*) * 7);
    if(!characteristics) {
        error_handler("Memory allocation failed.");
        return;
    }
    homekit_characteristic_change_callback_t* callback;
    characteristics[0] = NEW_HOMEKIT_CHARACTERISTIC(CURRENT_HEATING_COOLING_STATE, 0);
    characteristics[1] = NEW_HOMEKIT_CHARACTERISTIC(TARGET_HEATING_COOLING_STATE, 0);
    callback = calloc(1, sizeof(homekit_characteristic_change_callback_t));
    if(!callback) {
        error_handler("Memory allocation failed.");
        return;
    }
    callback->function = panasonic_ac_heating_cooling_state_callback;
    characteristics[1]->callback = callback;
    characteristics[2] = NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 25);
    characteristics[3] = NEW_HOMEKIT_CHARACTERISTIC(TARGET_TEMPERATURE, 25);
    *(characteristics[3]->min_value) = 16.0f;
    *(characteristics[3]->max_value) = 30.0f;
    *(characteristics[3]->min_step) = 1.0f;
    callback = calloc(1, sizeof(homekit_characteristic_change_callback_t));
    if(!callback) {
        error_handler("Memory allocation failed.");
        return;
    }
    callback->function = panasonic_ac_target_temperature_callback;
    characteristics[3]->callback = callback;
    characteristics[4] = NEW_HOMEKIT_CHARACTERISTIC(TEMPERATURE_DISPLAY_UNITS, 0);
    characteristics[5] = NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat");
    characteristics[6] = NULL;
    service->characteristics = characteristics;
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
    transmit_set_carrier(38000);
    uint16_t high = code >> 16, low = code;
    panasonic_ac_transmit_16(high);
    panasonic_ac_transmit_16(low);
}
void panasonic_ac_transmit_16(uint16_t code) {
    uint16_t header_mark = 3500;
    uint16_t header_space = 3500;
    uint16_t bit_mark = 870;
    uint16_t zero_space = 870;
    uint16_t one_space = 2600;
    uint16_t footer_space = 13900;

    uint8_t high = code >> 8, low = code;
    transmit_clear_time();
    transmit_mark(header_mark);
    transmit_space(header_space);
    transmit_code(high, 8, bit_mark, zero_space, one_space);
    transmit_code(high, 8, bit_mark, zero_space, one_space);
    transmit_code(low, 8, bit_mark, zero_space, one_space);
    transmit_code(low, 8, bit_mark, zero_space, one_space);
    transmit_mark(header_mark);
    transmit_space(header_space);
    transmit_code(high, 8, bit_mark, zero_space, one_space);
    transmit_code(high, 8, bit_mark, zero_space, one_space);
    transmit_code(low, 8, bit_mark, zero_space, one_space);
    transmit_code(low, 8, bit_mark, zero_space, one_space);
    transmit_mark(header_mark);
    transmit_space(header_space);
    transmit_mark(bit_mark);
    transmit_space(footer_space);
}