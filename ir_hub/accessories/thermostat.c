#include "espressif/esp_common.h"

#include "thermostat.h"
#include "../utilities/error_handler.h"

void thermostat_identify(homekit_value_t _value);

void thermostat_init(homekit_accessory_t* accessory) {
    accessory->category = homekit_accessory_category_thermostat;
    homekit_service_t** services = malloc(sizeof(homekit_service_t*) * 3);
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
    characteristics[0] = NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, thermostat_identify);
    characteristics[1] = NULL;
    service->characteristics = characteristics;
    services[0] = service;

    services[1] = NEW_HOMEKIT_SERVICE(THERMOSTAT, .primary=true);
    services[2] = NULL;
    accessory->services = services;
}

void thermostat_identify(homekit_value_t _value) {
    printf("Thermostat identify\n");
}