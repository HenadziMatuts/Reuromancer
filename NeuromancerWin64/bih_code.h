#ifndef _BIH_CODE_H
#define _BIH_CODE_H

#include <stdint.h>

/* this should be replaced with the 8086 emulator */

void vm_bih_call(uint16_t offt);

void sub105F6_bih_call(uint16_t offt);

#endif
