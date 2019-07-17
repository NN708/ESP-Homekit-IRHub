#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"

#include <wifi_config.h>
#include "utilities/error_handler.h"
#include "homekit_task.h"
#include "transmit.h"
#include "reset_button.h"

void on_wifi_event(wifi_config_event_t event);

TaskHandle_t homekit_init_task_handle;

void user_init(void) {
    uart_set_baud(0, 115200);
    error_led_init();
    reset_button_init();
    transmit_init();
    wifi_config_init2("IRHub", NULL, on_wifi_event);
    xTaskCreate(homekit_init_task, "Homekit Init", 640, NULL, 2, &homekit_init_task_handle);
}

void on_wifi_event(wifi_config_event_t event) {
    if(event == WIFI_CONFIG_CONNECTED) {
        xTaskNotify(homekit_init_task_handle, 0, eSetValueWithOverwrite);
    }
}