#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void homekit_add_accessory(homekit_server_config_t* config, homekit_accessory_t* accessory);
void homekit_add_service(homekit_accessory_t* accessory, homekit_service_t* service);
void homekit_add_characteristic(homekit_service_t* service, homekit_characteristic_t* characteristic);