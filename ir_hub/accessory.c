#include "espressif/esp_common.h"

#include "accessory.h"
#include "utilities/error_handler.h"

#include "accessories/panasonic_ac.h"
#include "accessories/bridge.h"

uint8_t accessory_count = 0;

homekit_accessory_t** accessories_init() { //TODO: Read accessories from flash, and extend accessories with realloc()
    homekit_accessory_t** accessories = malloc(sizeof(homekit_accessory_t*) * 5);
    if(!accessories) {
        error_handler("Memory allocation failed.");
        return NULL;
    }
    homekit_accessory_t* accessory;

    accessory_count++;
    accessory = NEW_HOMEKIT_ACCESSORY(.id=accessory_count);
    bridge_init(accessory);
    accessories[accessory_count - 1] = accessory;

    accessory_count++;
    accessory = NEW_HOMEKIT_ACCESSORY(.id=accessory_count);
    panasonic_ac_init(accessory);
    accessories[accessory_count - 1] = accessory;

    accessories[accessory_count] = NULL;
    return accessories;
}