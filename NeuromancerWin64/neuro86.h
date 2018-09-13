#ifndef I8086_CPU_H
#define I8086_CPU_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define MEM_SIZE 0x10000

enum state
{
	CPU_STOPPED, CPU_RUNNING, CPU_HALTED
};

enum flags
{
	F_CF = 0x0001, F_PF = 0x0004, F_AF = 0x0010, F_ZF = 0x0040, F_SF = 0x0080,
	F_TF = 0x0100, F_IF = 0x0200, F_DF = 0x0400, F_OF = 0x0800
};

enum regs16
{
	REG_AX, REG_CX, REG_DX, REG_BX, REG_SP, REG_BP, REG_SI, REG_DI, REG_COUNT
};

enum regs8
{
	REG_AL, REG_CL, REG_DL, REG_BL, REG_AH, REG_CH, REG_DH, REG_BH
};

typedef uint8_t(*pfn_far_call_cb)(uint16_t sp);

typedef struct cpu_s
{
	uint8_t regs[REG_COUNT * 2];
	uint16_t flags;
	uint16_t ip;
	pfn_far_call_cb callback;
	uint8_t state;
} cpu_t;

typedef struct modrm_s
{
	uint8_t rm: 3;
	uint8_t reg: 3;
	uint8_t mod: 2;
} modrm_t;

cpu_t *cpu_new(pfn_far_call_cb callback);
void cpu_reset(cpu_t *cpu, uint16_t addr);
void cpu_set_state(cpu_t *cpu, uint8_t state);
void cpu_run(cpu_t *cpu);
void cpu_destroy(cpu_t *cpu);

#endif