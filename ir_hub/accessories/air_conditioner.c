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
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TARGET_HEATER_COOLER_STATE, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(TEMPERATURE_DISPLAY_UNITS, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(NAME, "Air conditioner"));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(ROTATION_SPEED, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(SWING_MODE, 0));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(COOLING_THRESHOLD_TEMPERATURE, 30));
    homekit_add_characteristic(accessory->services[1], NEW_HOMEKIT_CHARACTERISTIC(HEATING_THRESHOLD_TEMPERATURE, 25));
}

void air_conditioner_identify(homekit_value_t _value) {
    printf("Air conditioner identify\n");
}