#include "transmit.h"
#include "pwm.h"

#define PIN_COUNT 1
uint8_t pins[PIN_COUNT] = {4};

void transmit_wait(uint16_t time);

uint32_t target_time;

void transmit_init() {
    pwm_init(PIN_COUNT, pins, false);
    pwm_set_duty(UINT16_MAX / 3);
}
void transmit_set_carrier(uint16_t frequency) {
    pwm_set_freq(frequency);
}

void transmit_mark(uint16_t time) {
    pwm_start();
    transmit_wait(time);
}
void transmit_space(uint16_t time) {
    pwm_stop();
    transmit_wait(time);
}

void transmit_clear_time() {
    target_time = sdk_system_get_time();
}
void transmit_wait(uint16_t time) {
    target_time += time;
    if (target_time >= time) {
        while (sdk_system_get_time() < target_time);
    } else {
        while (sdk_system_get_time() < target_time || sdk_system_get_time() >= (target_time - time));
    }
}

void transmit_code(uint32_t code, uint8_t length, uint16_t bit_mark, uint16_t zero_space, uint16_t one_space) {
    for (uint32_t mask = 1 << (length - 1); mask; mask >>= 1) {
        transmit_mark(bit_mark);
        if (code & mask) {
            transmit_space(one_space);
        } else {
            transmit_space(zero_space);
        }
    }
}