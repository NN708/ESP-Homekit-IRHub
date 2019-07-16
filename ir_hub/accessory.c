#include "espressif/esp_common.h"

#include "accessory.h"
#include "utilities/error_handler.h"
#include "utilities/homekit_utility.h"

#include "accessories/panasonic_ac.h"
#include "accessories/bridge.h"

void accessories_init(homekit_server_config_t* config) { //TODO: Read accessories from flash
    homekit_add_accessory(config, NEW_HOMEKIT_ACCESSORY());
    bridge_init(config->accessories[0]);

    homekit_add_accessory(config, NEW_HOMEKIT_ACCESSORY());
    air_conditioner_init(config->accessories[1]);
}