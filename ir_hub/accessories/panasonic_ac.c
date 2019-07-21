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
void panasonic_ac_target_heating_cooling_state_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context);
void panasonic_ac_target_temperature_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context);
void panasonic_ac_fan_active_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context);
uint32_t panasonic_ac_encode(homekit_accessory_t* accessory);
void panasonic_ac_transmit(uint32_t code);
void panasonic_ac_transmit_16(uint16_t code);
uint8_t reverse_bits(uint8_t num, uint8_t length);
void panasonic_ac_update_characteristic(homekit_accessory_t* accessory);

const uint8_t panasonic_ac_fan_speed_code[] = {0xF, 0x4, 0x2, 0x6}; // auto, low, medium, high

void panasonic_ac_init(homekit_accessory_t* accessory) {
    accessory->category = homekit_accessory_category_thermostat;
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION));
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(THERMOSTAT, .primary=true));
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(FAN2));
    homekit_add_linked_service(accessory->services[1], accessory->services[2]);

    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, panasonic_ac_identify));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Panasonic"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MODEL, "AC1")); // TODO: Fill in by user
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"));

    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_HEATING_COOLING_STATE, HEATING_COOLING_STATE_OFF));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_HEATING_COOLING_STATE, HEATING_COOLING_STATE_OFF, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(panasonic_ac_target_heating_cooling_state_callback)));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 25));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_TEMPERATURE, 25, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(panasonic_ac_target_temperature_callback)));
    *(accessory->services[1]->characteristics[3]->min_value) = 16.0f;
    *(accessory->services[1]->characteristics[3]->max_value) = 30.0f;
    *(accessory->services[1]->characteristics[3]->min_step) = 1.0f;
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TEMPERATURE_DISPLAY_UNITS, TEMPERATURE_DISPLAY_UNITS_CELSIUS));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"));

    homekit_add_characteristic(accessory->services[2], NEW_HOMEKIT_CHARACTERISTIC(ACTIVE, ACTIVE_INACTIVE, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(panasonic_ac_fan_active_callback)));
    homekit_add_characteristic(accessory->services[2], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Fan"));
    homekit_add_characteristic(accessory->services[2], NEW_HOMEKIT_CHARACTERISTIC(ROTATION_SPEED, 0));
    *(accessory->services[2]->characteristics[2]->min_step) = 33.333333f;
}


void panasonic_ac_identify(homekit_value_t _value) {
    printf("Panasonic AC identify\n");
}

void panasonic_ac_target_heating_cooling_state_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    uint8_t target_state = value.int_value;
    homekit_characteristic_t* active_characteristic = accessory->services[2]->characteristics[0];
    homekit_characteristic_t* rotation_speed_characteristic = accessory->services[2]->characteristics[2];
    uint8_t active = active_characteristic->value.int_value;

    if((target_state == HEATING_COOLING_STATE_OFF) && (active == ACTIVE_ACTIVE)) {
        rotation_speed_characteristic->value = HOMEKIT_FLOAT(0.0f);
        active_characteristic->value = HOMEKIT_UINT8(ACTIVE_INACTIVE);
        homekit_characteristic_notify(rotation_speed_characteristic, rotation_speed_characteristic->value);
        homekit_characteristic_notify(active_characteristic, active_characteristic->value);
    } else if((target_state != HEATING_COOLING_STATE_OFF) && (active == ACTIVE_INACTIVE)) {
        rotation_speed_characteristic->value = HOMEKIT_FLOAT(33.333333f);
        active_characteristic->value = HOMEKIT_UINT8(ACTIVE_ACTIVE);
        homekit_characteristic_notify(rotation_speed_characteristic, rotation_speed_characteristic->value);
        homekit_characteristic_notify(active_characteristic, active_characteristic->value);
    } else {
        panasonic_ac_transmit(panasonic_ac_encode(accessory));
        panasonic_ac_update_characteristic(accessory);
    }
}

void panasonic_ac_target_temperature_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    panasonic_ac_transmit(panasonic_ac_encode(accessory));
    panasonic_ac_update_characteristic(accessory);
}

void panasonic_ac_fan_active_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    uint8_t active = value.int_value;
    homekit_characteristic_t* target_state_characteristic = accessory->services[1]->characteristics[1];
    uint8_t target_state = target_state_characteristic->value.int_value;

    if((active == ACTIVE_INACTIVE) && (target_state != HEATING_COOLING_STATE_OFF)) {
        target_state_characteristic->value = HOMEKIT_UINT8(HEATING_COOLING_STATE_OFF);
        homekit_characteristic_notify(target_state_characteristic, target_state_characteristic->value);
    } else if((active == ACTIVE_ACTIVE) && (target_state == HEATING_COOLING_STATE_OFF)) {
        target_state_characteristic->value = HOMEKIT_UINT8(HEATING_COOLING_STATE_AUTO);
        homekit_characteristic_notify(target_state_characteristic, target_state_characteristic->value);
    } else {
        panasonic_ac_transmit(panasonic_ac_encode(accessory));
        panasonic_ac_update_characteristic(accessory);
    }
}

uint32_t panasonic_ac_encode(homekit_accessory_t* accessory) {
    uint8_t current_state = accessory->services[1]->characteristics[0]->value.int_value;
    uint8_t target_state = accessory->services[1]->characteristics[1]->value.int_value;
    uint8_t target_temperature = (int)accessory->services[1]->characteristics[3]->value.float_value;
    float rotation_speed = accessory->services[2]->characteristics[2]->value.float_value;
    uint32_t code = 0x00002F6C;
    uint8_t state_code = 0, temperature_code = 0;

    uint8_t state = ((target_state != HEATING_COOLING_STATE_OFF) ? target_state : current_state); // when changed to off, read current state
    switch(state) {
    case HEATING_COOLING_STATE_OFF:
    case HEATING_COOLING_STATE_AUTO:
        state_code = 3; // auto
        temperature_code = 8;
        break;
    case HEATING_COOLING_STATE_HEAT:
        state_code = 1; // heat
        temperature_code = reverse_bits(target_temperature - 15, 4);
        break;
    case HEATING_COOLING_STATE_COOL:
        state_code = 2; // cool
        temperature_code = reverse_bits(target_temperature - 15, 4);
        break;
    }

    bool toggle_power = ((target_state == HEATING_COOLING_STATE_OFF) && (current_state != HEATING_COOLING_STATE_OFF)) || ((current_state == HEATING_COOLING_STATE_OFF) && (target_state != HEATING_COOLING_STATE_OFF));
    if(!toggle_power) code += 1 << 20;
    code += state_code << 21;
    code += panasonic_ac_fan_speed_code[(int)(rotation_speed / 33)] << 24;
    code += temperature_code << 28;
    return code;
}

void panasonic_ac_transmit(uint32_t code) {
    transmit_set_carrier(CARRIER_FREQUENCY);
    uint16_t code_16[] = {code >> 16, code};
    for(uint8_t i = 0; i < 2; i++) {
        uint8_t code_8[] = {code_16[i] >> 8, code_16[i]};
        transmit_clear_time();
        transmit_mark(HEADER_MARK);
        transmit_space(HEADER_SPACE);
        for(uint8_t j = 0; j < 2; j++) {
            for(uint8_t k = 0; k < 2; k++) {
                transmit_code(code_8[k], 8, BIT_MARK, ZERO_SPACE, ONE_SPACE);
                transmit_code(code_8[k], 8, BIT_MARK, ZERO_SPACE, ONE_SPACE);
            }
            transmit_mark(HEADER_MARK);
            transmit_space(HEADER_SPACE);
        }
        transmit_mark(BIT_MARK);
        transmit_space(FOOTER_SPACE);
    }
}

uint8_t reverse_bits(uint8_t num, uint8_t length) {
    uint8_t reverse_num = 0;
    for(uint8_t i = 0; i < length; i++) {
        reverse_num <<= 1;
        reverse_num += num % 2;
        num >>= 1;
    }
    return reverse_num;
}

void panasonic_ac_update_characteristic(homekit_accessory_t* accessory) {
    homekit_service_t* thermostat_service = accessory->services[1];

    homekit_characteristic_t* current_state_characteristic = thermostat_service->characteristics[0];
    uint8_t target_state = thermostat_service->characteristics[1]->value.int_value;
    if(target_state == HEATING_COOLING_STATE_AUTO) target_state = HEATING_COOLING_STATE_OFF; // auto is not a valid value for current heating cooling state
    current_state_characteristic->value = HOMEKIT_UINT8(target_state);
    homekit_characteristic_notify(current_state_characteristic, current_state_characteristic->value);

    homekit_characteristic_t* current_temperature_characteristic = thermostat_service->characteristics[2];
    float target_temperature = thermostat_service->characteristics[3]->value.float_value;
    current_temperature_characteristic->value = HOMEKIT_FLOAT(target_temperature);
    homekit_characteristic_notify(current_temperature_characteristic, current_temperature_characteristic->value);
}