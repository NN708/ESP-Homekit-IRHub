#include <homekit/homekit.h>
#include <homekit/characteristics.h>

/* macros */
#define HEATING_COOLING_STATE_OFF 0
#define HEATING_COOLING_STATE_HEAT 1
#define HEATING_COOLING_STATE_COOL 2
#define HEATING_COOLING_STATE_AUTO 3

/* dynamic memory functions */
void homekit_add_accessory(homekit_server_config_t* config, homekit_accessory_t* accessory);
void homekit_add_service(homekit_accessory_t* accessory, homekit_service_t* service);
void homekit_add_characteristic(homekit_service_t* service, homekit_characteristic_t* characteristic);