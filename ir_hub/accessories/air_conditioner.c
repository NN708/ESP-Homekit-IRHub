#include "espressif/esp_common.h"

#include "air_conditioner.h"
#include "../utilities/error_handler.h"
#include "../utilities/homekit_utility.h"

void air_conditioner_identify(homekit_value_t _value);

void air_conditioner_init(homekit_accessory_t* accessory) {
    accessory->category = homekit_accessory_category_air_conditioner;
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION));
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(HEATER_COOLER, .primary=true));

    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, air_conditioner_identify));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Ob"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MODEL, "ObAC1"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Air conditioner"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"));

    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(ACTIVE, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 25));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(CURRENT_HEATER_COOLER_STATE, 0));
    *(accessory->services[1]->characteristics[2]->max_value) = 2.0f;
    accessory->services[1]->characteristics[2]->valid_values.count = 3;
    accessory->services[1]->characteristics[2]->valid_values.values = (uint8_t[]) { 0, 1, 2 };
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_HEATER_COOLER_STATE, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TEMPERATURE_DISPLAY_UNITS, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Air conditioner"));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(ROTATION_SPEED, 0));
    *(accessory->services[1]->characteristics[6]->min_step) = 25.0f;
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(SWING_MODE, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(COOLING_THRESHOLD_TEMPERATURE, 30));
    *(accessory->services[1]->characteristics[8]->min_value) = 16.0f;
    *(accessory->services[1]->characteristics[8]->max_value) = 30.0f;
    *(accessory->services[1]->characteristics[8]->min_step) = 1.0f;
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(HEATING_THRESHOLD_TEMPERATURE, 25));
    *(accessory->services[1]->characteristics[9]->min_value) = 16.0f;
    *(accessory->services[1]->characteristics[9]->max_value) = 30.0f;
    *(accessory->services[1]->characteristics[9]->min_step) = 1.0f;
}

void air_conditioner_identify(homekit_value_t _value) {
    printf("Air conditioner identify\n");
}