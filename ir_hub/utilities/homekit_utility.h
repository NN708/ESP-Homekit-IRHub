#include <homekit/homekit.h>
#include <homekit/characteristics.h>

/* macros */
#define HEATING_COOLING_STATE_OFF 0
#define HEATING_COOLING_STATE_HEAT 1
#define HEATING_COOLING_STATE_COOL 2
#define HEATING_COOLING_STATE_AUTO 3

#define TEMPERATURE_DISPLAY_UNITS_CELSIUS 0
#define TEMPERATURE_DISPLAY_UNITS_FAHRENHEIT 1

#define SLAT_STATE_FIXED 0
#define SLAT_STATE_JAMMED 1
#define SLAT_STATE_SWINGING 2

#define SLAT_TYPE_HORIZONTAL 0
#define SLAT_TYPE_VERTICAL 1

#define ACTIVE_INACTIVE 0
#define ACTIVE_ACTIVE 1

/* dynamic memory functions */
void homekit_add_accessory(homekit_server_config_t* config, homekit_accessory_t* accessory);
void homekit_add_service(homekit_accessory_t* accessory, homekit_service_t* service);
void homekit_add_characteristic(homekit_service_t* service, homekit_characteristic_t* characteristic);
void homekit_add_linked_service(homekit_service_t* service1, homekit_service_t* service2);