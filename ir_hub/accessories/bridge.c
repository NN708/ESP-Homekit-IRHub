#include "espressif/esp_common.h"

#include "bridge.h"
#include "../utilities/error_handler.h"
#include "../utilities/homekit_utility.h"

void bridge_identify(homekit_value_t _value);

void bridge_init(homekit_accessory_t* accessory) {
    accessory->category = homekit_accessory_category_bridge;
    homekit_add_service(accessory, NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION));

    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, bridge_identify));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Ob"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(MODEL, "ObIRHub1"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(NAME, "IRHub"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01"));
    homekit_add_characteristic(accessory->services[0], NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"));
}

void bridge_identify(homekit_value_t _value) {
    printf("Bridge identify\n");
}