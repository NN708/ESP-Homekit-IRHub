#include "espressif/esp_common.h"
#include "esp8266.h"
#include "error_handler.h"

const int led_gpio = 2;

void error_led_init() {
    gpio_enable(led_gpio, GPIO_OUTPUT);
    gpio_write(led_gpio, true);
}

void error_handler(const char* message) {
    printf("Error: %s\n", message);
    gpio_write(led_gpio, false);
    while (true);
}

void warning_handler(const char* message) {
    printf("Warning: %s\n", message);
    gpio_write(led_gpio, false);
}