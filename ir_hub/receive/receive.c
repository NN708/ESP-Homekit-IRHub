#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

void gpio_intr_handler(uint8_t gpio_num);
void receiveTask(void *pvParameters);

/* pin config */
const uint8_t gpio = 5;

uint32_t lastTick = 0;
uint8_t count = 0;

void user_init(void) {
    uart_set_baud(0, 115200);
    printf("Initialization\n");
    gpio_enable(gpio, GPIO_INPUT);
    
    xTaskCreate(receiveTask, "receiveTask", 256, NULL, 1, NULL);
}

void receiveTask(void *pvParameters) {
    gpio_set_interrupt(gpio, GPIO_INTTYPE_EDGE_ANY, gpio_intr_handler);
    while(true);
}

void gpio_intr_handler(uint8_t gpio_num) {
    uint32_t now = sdk_system_get_time();
    uint32_t deltaTime = now - lastTick;
    lastTick = now;
    if(gpio_read(gpio)) {
        printf("High ");
    } else {
        printf("Low  ");
    }
    printf("%d\n", deltaTime);
}