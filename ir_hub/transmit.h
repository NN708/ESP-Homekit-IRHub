#include "espressif/esp_common.h"

void transmit_init();
void transmit_set_carrier(uint16_t frequency);
void transmit_mark(uint16_t time);
void transmit_space(uint16_t time);
void transmit_clear_time();
void transmit_code(uint32_t code, uint8_t length, uint16_t bit_mark, uint16_t zero_space, uint16_t one_space);