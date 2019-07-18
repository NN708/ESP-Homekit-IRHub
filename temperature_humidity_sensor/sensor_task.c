#include "espressif/esp_common.h"
#include "FreeRTOS.h"
#include "task.h"

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <dht/dht.h>

#include "sensor_task.h"

#define SENSOR_PIN 4

void temperature_humidity_sensor_identify(homekit_value_t _value);

/* homekit config */
homekit_characteristic_t temperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 0);
homekit_characteristic_t humidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);
homekit_accessory_t* accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_sensor, .services=(homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(IDENTIFY, temperature_humidity_sensor_identify),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Ob"),
            HOMEKIT_CHARACTERISTIC(MODEL, "ObTH1"),
            HOMEKIT_CHARACTERISTIC(NAME, "Temperature and Humidity Sensor"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "01"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
            NULL
        }),
        HOMEKIT_SERVICE(TEMPERATURE_SENSOR, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Temperature Sensor"),
            &temperature,
            NULL
        }),
        HOMEKIT_SERVICE(HUMIDITY_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Humidity Sensor"),
            &humidity,
            NULL
        }),
        NULL
    }),
    NULL
};
homekit_server_config_t homekit_server_config = {
    .accessories = accessories,
    .password = "111-11-111"
};

void temperature_humidity_sensor_task(void *parameters) {
    gpio_set_pullup(SENSOR_PIN, false, false);
    homekit_server_init(&homekit_server_config);

    float humidity_value, temperature_value;
    while(true) {
        if(dht_read_float_data(DHT_TYPE_DHT11, SENSOR_PIN, &humidity_value, &temperature_value)) {
            temperature.value.float_value = temperature_value;
            humidity.value.float_value = humidity_value;
            homekit_characteristic_notify(&temperature, temperature.value);
            homekit_characteristic_notify(&humidity, humidity.value);
        }
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void temperature_humidity_sensor_identify(homekit_value_t _value) {
    printf("Temperature and humidity sensor identify\n");
}