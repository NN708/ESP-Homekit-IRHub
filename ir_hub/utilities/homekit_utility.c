#include "espressif/esp_common.h"

#include "homekit_utility.h"
#include "error_handler.h"

void homekit_add_service(homekit_accessory_t* accessory, homekit_service_t* service) {
    uint8_t service_count = 0;
    if(accessory->services) {
        while(accessory->services[service_count]) {
            service_count++;
        }
    }
    service_count++;
    accessory->services = realloc(accessory->services, sizeof(homekit_service_t*) * (service_count + 1));
    if(!accessory->services) {
        error_handler("Memory allocation failed.");
        return;
    }
    accessory->services[service_count - 1] = service;
    accessory->services[service_count] = NULL;
}

void homekit_add_characteristic(homekit_service_t* service, homekit_characteristic_t* characteristic) {
    uint8_t characteristic_count = 0;
    if(service->characteristics) {
        while(service->characteristics[characteristic_count]) {
            characteristic_count++;
        }
    }
    characteristic_count++;
    service->characteristics = realloc(service->characteristics, sizeof(homekit_characteristic_t*) * (characteristic_count + 1));
    if(!service->characteristics) {
        error_handler("Memory allocation failed.");
        return;
    }
    service->characteristics[characteristic_count - 1] = characteristic;
    service->characteristics[characteristic_count] = NULL;
}