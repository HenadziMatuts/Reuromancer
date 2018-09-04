#include <stdint.h>

uint8_t* translate_x16_to_x64(uint16_t seg, uint16_t offt);
void translate_x64_to_x16(uint8_t *src, uint16_t *seg, uint16_t *offt);
