#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

#include <wifi_config.h>
#include "utilities/error_handler.h"
#include "accessory.h"
#include "transmit.h"

void on_wifi_event(wifi_config_event_t event);

homekit_server_config_t config;

void user_init(void) {
    uart_set_baud(0, 115200);
    error_led_init();
    transmit_init();
    wifi_config_init2("IRHub", NULL, on_wifi_event);
}

void on_wifi_event(wifi_config_event_t event) {
    if(event == WIFI_CONFIG_CONNECTED) {
        config.accessories = accessories_init();
        config.password = "111-11-111";
        homekit_server_init(&config);
    }
}