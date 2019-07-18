#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"

#include <wifi_config.h>

#include "sensor_task.h"

void on_wifi_event(wifi_config_event_t event);

void user_init(void) {
    uart_set_baud(0, 115200);
    //error_led_init();
    //reset_button_init();
    wifi_config_init2("Sensor", NULL, on_wifi_event);
}

void on_wifi_event(wifi_config_event_t event) {
    if(event == WIFI_CONFIG_CONNECTED) {
        xTaskCreate(temperature_humidity_sensor_task, "Temperature Humidity Sensor", 256, NULL, 2, NULL);
    }
}