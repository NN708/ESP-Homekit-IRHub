#include "espressif/esp_common.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "esp8266.h"

#include <wifi_config.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "reset_button.h"

void gpio_intr_handler(uint8_t gpio_num);

const int button_gpio = 5;
uint32_t press_time;

void reset_button_init() {
    gpio_enable(button_gpio, GPIO_INPUT);
    gpio_set_interrupt(button_gpio, GPIO_INTTYPE_EDGE_ANY, gpio_intr_handler);
}

void gpio_intr_handler(uint8_t gpio_num)
{
    if(gpio_read(button_gpio)) { // pos
        press_time = xTaskGetTickCountFromISR();
    } else { // neg
        uint32_t delta_time, now;
        now = xTaskGetTickCountFromISR();
        if(press_time > now) {
            delta_time = UINT32_MAX - press_time + now;
        } else {
            delta_time = now - press_time;
        }
        if(delta_time * portTICK_PERIOD_MS >= 5000) { // 5 seconds
            homekit_server_reset();
            wifi_config_reset();
            sdk_system_restart();
        }
    }
    sdk_os_delay_us(100);
}