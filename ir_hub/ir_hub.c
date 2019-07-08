#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

#include "utilities/error_handler.h"
#include "accessory.h"
#include "transmit.h"
#include "config.h"

static void wifi_init();
void panasonic_ac_transmit(uint32_t code);

homekit_server_config_t config;

void user_init(void) {
    uart_set_baud(0, 115200);
    error_led_init();
    wifi_init();
    transmit_init();

    /* homekit init */
    config.accessories = accessories_init();
    config.password = "111-11-111";
    homekit_server_init(&config);
}

static void wifi_init() {
    struct sdk_station_config wifi_config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASSWORD,
    };
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&wifi_config);
    sdk_wifi_station_connect();
}