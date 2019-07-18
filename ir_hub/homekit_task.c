#include "espressif/esp_common.h"
#include "FreeRTOS.h"
#include "task.h"

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#include "homekit_task.h"
#include "utilities/error_handler.h"
#include "utilities/homekit_utility.h"

#include "accessories/panasonic_ac.h"
#include "accessories/bridge.h"

homekit_server_config_t homekit_server_config;

void homekit_init_task(void *parameters) {
    homekit_server_config.password = "111-11-111";

    homekit_add_accessory(&homekit_server_config, NEW_HOMEKIT_ACCESSORY()); //TODO: Read accessories from flash
    bridge_init(homekit_server_config.accessories[0]);

    homekit_add_accessory(&homekit_server_config, NEW_HOMEKIT_ACCESSORY());
    panasonic_ac_init(homekit_server_config.accessories[1]);

    homekit_server_init(&homekit_server_config);

    vTaskDelete(NULL);
}