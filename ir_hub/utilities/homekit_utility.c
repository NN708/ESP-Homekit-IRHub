#include "espressif/esp_common.h"

#include "homekit_utility.h"
#include "error_handler.h"

void homekit_add_accessory(homekit_server_config_t* config, homekit_accessory_t* accessory) {
    uint8_t accessory_count = 0;
    if(config->accessories) {
        while(config->accessories[accessory_count]) {
            accessory_count++;
        }
    }
    accessory_count++;
    config->accessories = realloc(config->accessories, sizeof(homekit_accessory_t*) * (accessory_count + 1));
    if(!config->accessories) {
        error_handler("Memory allocation failed.");
        return;
    }
    accessory->id = accessory_count;
    config->accessories[accessory_count - 1] = accessory;
    config->accessories[accessory_count] = NULL;
}

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

void homekit_add_linked_service(homekit_service_t* service1, homekit_service_t* service2) {
    uint8_t service_count = 0;
    if(service1->linked) {
        while(service1->linked[service_count]) {
            service_count++;
        }
    }
    service_count++;
    service1->linked = realloc(service1->linked, sizeof(homekit_service_t*) * (service_count + 1));
    if(!service1->linked) {
        error_handler("Memory allocation failed.");
        return;
    }
    service1->linked[service_count - 1] = service2;
    service1->linked[service_count] = NULL;
}