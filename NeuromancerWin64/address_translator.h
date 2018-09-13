#include <stdint.h>

#define SEG_000 0
#define SEG_001 1
#define SEG_002 2
#define SEG_003 3
#define SEG_004 4
#define SEG_005 5
#define SEG_006 6
#define SEG_007 7
#define SEG_008 8
#define SEG_009 9
#define SEG_010 10
#define SEG_011 11
#define SEG_012 12
#define SEG_013 13
#define SEG_014 14
#define SEG_015 15
#define SEG_016 16
#define DSEG    17

#define STACK_OFFT 0xCC10
#define STACK_SIZE 0x800

uint8_t* translate_x16_to_x64(uint16_t seg, uint16_t offt);
void translate_x64_to_x16(uint8_t *src, uint16_t *seg, uint16_t *offt);
