#include "espressif/esp_common.h"

#include "homekit_utility.h"
#include "error_handler.h"

homekit_characteristic_change_callback_t* new_homekit_callback(homekit_characteristic_change_callback_fn function) {
    homekit_characteristic_change_callback_t* callback;
    callback = calloc(1, sizeof(homekit_characteristic_change_callback_t));
    if(!callback) {
        error_handler("Memory allocation failed.");
        return;
    }
    callback->function = function;
    return callback;
}