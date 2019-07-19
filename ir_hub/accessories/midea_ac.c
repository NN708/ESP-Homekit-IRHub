#include "espressif/esp_common.h"

#include "transmit.h"
#include "midea_ac.h"
#include "../utilities/error_handler.h"
#include "../utilities/homekit_utility.h"

#define CARRIER_FREQUENCY 38000
#define HEADER_MARK 4500
#define HEADER_SPACE 4500
#define BIT_MARK 560
#define ZERO_SPACE 560
#define ONE_SPACE 1680
#define FOOTER_SPACE 4500

void midea_ac_identify(homekit_value_t _value);
void midea_ac_target_heating_cooling_state_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context);
void midea_ac_target_temperature_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context);
uint32_t midea_ac_encode(homekit_accessory_t* accessory);
void midea_ac_transmit(uint32_t code);

void midea_ac_init(homekit_accessory_t* accessory) {
    accessory->category = homekit_accessory_category_thermostat;
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION));
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(THERMOSTAT, .primary=true));

    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, midea_ac_identify));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Midea"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MODEL, "AC1")); // TODO: Fill in by user
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"));

    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_HEATING_COOLING_STATE, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_HEATING_COOLING_STATE, 0, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(midea_ac_target_heating_cooling_state_callback)));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 25));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_TEMPERATURE, 25, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(midea_ac_target_temperature_callback)));
    *(accessory->services[1]->characteristics[3]->min_value) = 17.0f;
    *(accessory->services[1]->characteristics[3]->max_value) = 30.0f;
    *(accessory->services[1]->characteristics[3]->min_step) = 1.0f;
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TEMPERATURE_DISPLAY_UNITS, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"));
}


void midea_ac_identify(homekit_value_t _value) {
    printf("Midea AC identify\n");
}

void midea_ac_target_heating_cooling_state_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    uint8_t target_state = value.int_value;
    homekit_characteristic_t* current_state_characteristic = characteristic->service->characteristics[0];

    midea_ac_transmit(midea_ac_encode(accessory));
    if(target_state == HEATING_COOLING_STATE_AUTO) target_state = HEATING_COOLING_STATE_OFF; // auto is not a valid value for current heating cooling state
    current_state_characteristic->value = HOMEKIT_UINT8(target_state);
    homekit_characteristic_notify(current_state_characteristic, current_state_characteristic->value);
}

void midea_ac_target_temperature_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    float target_temperature = value.float_value;
    homekit_characteristic_t* current_temperature_characteristic = characteristic->service->characteristics[2];

    midea_ac_transmit(midea_ac_encode(accessory));
    current_temperature_characteristic->value = HOMEKIT_FLOAT(target_temperature);
    homekit_characteristic_notify(current_temperature_characteristic, current_temperature_characteristic->value);
}

uint32_t midea_ac_encode(homekit_accessory_t* accessory) {
    uint8_t target_state = accessory->services[1]->characteristics[1]->value.int_value;
    uint32_t code;
    if(target_state == HEATING_COOLING_STATE_OFF) {
        code = 0xB27BE0; // off state code
    } else {
        code = 0xB21F00;

    }
    return code;
}

void midea_ac_transmit(uint32_t code) {
    transmit_set_carrier(CARRIER_FREQUENCY);

}