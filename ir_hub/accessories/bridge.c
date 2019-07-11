#include "espressif/esp_common.h"

#include "bridge.h"
#include "../utilities/error_handler.h"

void bridge_identify(homekit_value_t _value);

void bridge_init(homekit_accessory_t* accessory) {
    accessory->category = homekit_accessory_category_bridge;
    homekit_service_t** services = malloc(sizeof(homekit_service_t*) * 2);
    if(!services) {
        error_handler("Memory allocation failed.");
        return;
    }

    homekit_service_t* service;
    homekit_characteristic_t** characteristics;

    service = NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION);
    characteristics = malloc(sizeof(homekit_characteristic_t*) * 7);
    if(!characteristics) {
        error_handler("Memory allocation failed.");
        return;
    }
    characteristics[0] = NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, bridge_identify);
    characteristics[1] = NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Ob");
    characteristics[2] = NEW_HOMEKIT_CHARACTERISTIC(MODEL, "ObIRHub1");
    characteristics[3] = NEW_HOMEKIT_CHARACTERISTIC(NAME, "IRHub");
    characteristics[4] = NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01");
    characteristics[5] = NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1");
    characteristics[6] = NULL;
    service->characteristics = characteristics;
    services[0] = service;

    services[1] = NULL;
    accessory->services = services;
}

void bridge_identify(homekit_value_t _value) {
    printf("Bridge identify\n");
}