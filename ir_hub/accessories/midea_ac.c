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
void midea_ac_fan_active_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context);
uint32_t midea_ac_encode(homekit_accessory_t* accessory);
void midea_ac_transmit(uint32_t code);
void midea_ac_update_characteristic(homekit_accessory_t* accessory);

const uint8_t midea_ac_temperature_code[] = { // 17 to 30
    0x0, 0x1, 0x3, 0x2,
    0x6, 0x7, 0x5, 0x4, 0xC,
    0xD, 0x9, 0x8, 0xA, 0xB
};
const uint8_t midea_ac_fan_speed_code[] = {0, 5, 4, 2, 1}; // auto, 1 - 4

void midea_ac_init(homekit_accessory_t* accessory) {
    accessory->category = homekit_accessory_category_thermostat;
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION));
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(THERMOSTAT, .primary=true));
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(FAN2));
    homekit_add_linked_service(accessory->services[1], accessory->services[2]);

    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, midea_ac_identify));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Midea"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MODEL, "AC1")); // TODO: Fill in by user
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"));

    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_HEATING_COOLING_STATE, HEATING_COOLING_STATE_OFF));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_HEATING_COOLING_STATE, HEATING_COOLING_STATE_OFF, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(midea_ac_target_heating_cooling_state_callback)));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 25));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_TEMPERATURE, 25, .min_value=(float[]){17}, .max_value=(float[]){30}, .min_step=(float[]){1}, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(midea_ac_target_temperature_callback)));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TEMPERATURE_DISPLAY_UNITS, TEMPERATURE_DISPLAY_UNITS_CELSIUS));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"));

    homekit_add_characteristic(accessory->services[2], NEW_HOMEKIT_CHARACTERISTIC(ACTIVE, ACTIVE_INACTIVE, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(midea_ac_fan_active_callback)));
    homekit_add_characteristic(accessory->services[2], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Fan"));
    homekit_add_characteristic(accessory->services[2], NEW_HOMEKIT_CHARACTERISTIC(ROTATION_SPEED, 0, .min_step=(float[]){25}));
}


void midea_ac_identify(homekit_value_t _value) {
    printf("Midea AC identify\n");
}

void midea_ac_target_heating_cooling_state_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    uint8_t target_state = value.int_value;
    homekit_characteristic_t* active_characteristic = accessory->services[2]->characteristics[0];
    homekit_characteristic_t* rotation_speed_characteristic = accessory->services[2]->characteristics[2];
    uint8_t active = active_characteristic->value.int_value;

    if ((target_state == HEATING_COOLING_STATE_OFF) && (active == ACTIVE_ACTIVE)) {
        rotation_speed_characteristic->value = HOMEKIT_FLOAT(0.0f);
        active_characteristic->value = HOMEKIT_UINT8(ACTIVE_INACTIVE);
        homekit_characteristic_notify(rotation_speed_characteristic, rotation_speed_characteristic->value);
        homekit_characteristic_notify(active_characteristic, active_characteristic->value);
    } else if ((target_state != HEATING_COOLING_STATE_OFF) && (active == ACTIVE_INACTIVE)) {
        rotation_speed_characteristic->value = HOMEKIT_FLOAT(50.0f);
        active_characteristic->value = HOMEKIT_UINT8(ACTIVE_ACTIVE);
        homekit_characteristic_notify(rotation_speed_characteristic, rotation_speed_characteristic->value);
        homekit_characteristic_notify(active_characteristic, active_characteristic->value);
    } else {
        midea_ac_transmit(midea_ac_encode(accessory));
        midea_ac_update_characteristic(accessory);
    }
}

void midea_ac_target_temperature_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    midea_ac_transmit(midea_ac_encode(accessory));
    midea_ac_update_characteristic(accessory);
}

void midea_ac_fan_active_callback(homekit_characteristic_t* characteristic, homekit_value_t value, void* context) {
    homekit_accessory_t* accessory = characteristic->service->accessory;
    uint8_t active = value.int_value;
    homekit_characteristic_t* target_state_characteristic = accessory->services[1]->characteristics[1];
    uint8_t target_state = target_state_characteristic->value.int_value;

    if ((active == ACTIVE_INACTIVE) && (target_state != HEATING_COOLING_STATE_OFF)) {
        target_state_characteristic->value = HOMEKIT_UINT8(HEATING_COOLING_STATE_OFF);
        homekit_characteristic_notify(target_state_characteristic, target_state_characteristic->value);
    } else {
        midea_ac_transmit(midea_ac_encode(accessory));
        midea_ac_update_characteristic(accessory);
    }
}

uint32_t midea_ac_encode(homekit_accessory_t* accessory) {
    uint8_t target_state = accessory->services[1]->characteristics[1]->value.int_value;
    uint8_t target_temperature = (int)accessory->services[1]->characteristics[3]->value.float_value;
    float rotation_speed = accessory->services[2]->characteristics[2]->value.float_value;
    uint8_t active = accessory->services[2]->characteristics[0]->value.int_value;
    uint32_t code;
    uint8_t state_code = 0, temperature_code = 0, fan_speed_code = 0;

    if (active == ACTIVE_INACTIVE) {
        code = 0xB27BE0; // off state code
    } else {
        code = 0xB21F00;
        switch (target_state) {
        case HEATING_COOLING_STATE_OFF: // fan mode
            state_code = 1;
            temperature_code = 0xE;
            fan_speed_code = midea_ac_fan_speed_code[(int)(rotation_speed / 25)];
            break;
        case HEATING_COOLING_STATE_HEAT:
            state_code = 3;
            temperature_code = midea_ac_temperature_code[target_temperature - 17];
            fan_speed_code = midea_ac_fan_speed_code[(int)(rotation_speed / 25)];
            break;
        case HEATING_COOLING_STATE_COOL:
            state_code = 0;
            temperature_code = midea_ac_temperature_code[target_temperature - 17];
            fan_speed_code = midea_ac_fan_speed_code[(int)(rotation_speed / 25)];
            break;
        case HEATING_COOLING_STATE_AUTO:
            state_code = 2;
            temperature_code = midea_ac_temperature_code[target_temperature - 17];
            fan_speed_code = 0;
            break;
        }
        code += state_code << 2;
        code += temperature_code << 4;
        code += fan_speed_code << 13;
    }
    return code;
}

void midea_ac_transmit(uint32_t code) {
    transmit_set_carrier(CARRIER_FREQUENCY);
    uint8_t code_8[] = {code >> 16, code >> 8, code};
    transmit_clear_time();
    for (uint8_t i = 0; i < 2; i++) {
        transmit_mark(HEADER_MARK);
        transmit_space(HEADER_SPACE);
        for (uint8_t j = 0; j < 3; j++) {
            transmit_code(code_8[j], 8, BIT_MARK, ZERO_SPACE, ONE_SPACE);
            transmit_code(!code_8[j], 8, BIT_MARK, ZERO_SPACE, ONE_SPACE);
        }
        transmit_mark(BIT_MARK);
        transmit_space(FOOTER_SPACE);
    }
}

void midea_ac_update_characteristic(homekit_accessory_t* accessory) {
    homekit_service_t* thermostat_service = accessory->services[1];

    homekit_characteristic_t* current_state_characteristic = thermostat_service->characteristics[0];
    uint8_t target_state = thermostat_service->characteristics[1]->value.int_value;
    if (target_state == HEATING_COOLING_STATE_AUTO) target_state = HEATING_COOLING_STATE_OFF; // auto is not a valid value for current heating cooling state
    current_state_characteristic->value = HOMEKIT_UINT8(target_state);
    homekit_characteristic_notify(current_state_characteristic, current_state_characteristic->value);

    homekit_characteristic_t* current_temperature_characteristic = thermostat_service->characteristics[2];
    float target_temperature = thermostat_service->characteristics[3]->value.float_value;
    current_temperature_characteristic->value = HOMEKIT_FLOAT(target_temperature);
    homekit_characteristic_notify(current_temperature_characteristic, current_temperature_characteristic->value);
}