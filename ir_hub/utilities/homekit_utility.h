#include <homekit/homekit.h>
#include <homekit/characteristics.h>

homekit_characteristic_change_callback_t* new_homekit_callback(homekit_characteristic_change_callback_fn function);