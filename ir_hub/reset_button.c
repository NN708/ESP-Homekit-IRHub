#include "espressif/esp_common.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "esp8266.h"
#include "reset_button.h"

void gpio_interrupt_handler(uint8_t gpio_num);

const int button_gpio = 5;

void reset_button_init() {
    gpio_enable(button_gpio, GPIO_INPUT);
    gpio_set_interrupt(button_gpio, GPIO_INTTYPE_EDGE_POS, gpio_interrupt_handler);
}

void gpio_interrupt_handler(uint8_t gpio_num)
{

}