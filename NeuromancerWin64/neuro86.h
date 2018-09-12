#ifndef I8086_CPU_H
#define I8086_CPU_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define MEM_SIZE 0x10000
#define STACK_SIZE 0x100

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

typedef struct cpu_s
{
	uint8_t regs[REG_COUNT * 2];
	uint16_t flags;
	uint16_t ip;
	void (*callback)(struct cpu_s *cpu, uint16_t sp);
} cpu_t;

typedef struct modrm_s
{
	uint8_t rm: 3;
	uint8_t reg: 3;
	uint8_t mod: 2;
} modrm_t;

cpu_t *cpu_new(uint16_t addr, void(*callback)(cpu_t *cpu, uint16_t sp));
void cpu_reset(cpu_t *cpu, uint16_t addr);
void cpu_run(cpu_t *cpu);
void cpu_destroy(cpu_t **cpu);

#endif