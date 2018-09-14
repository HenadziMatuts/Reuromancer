#include <stdio.h>

#include "address_translator.h"
#include "neuro86.h"

static const uint8_t tbl_pf[0x100] = {
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

// reg

static inline uint8_t get_reg_u8(cpu_t *cpu, uint8_t reg)
{
	return cpu->regs[((reg & 3) << 1) + ((reg & 4) >> 2)];
}

static inline void set_reg_u8(cpu_t *cpu, uint8_t reg, uint8_t val)
{
	cpu->regs[((reg & 3) << 1) + ((reg & 4) >> 2)] = val;
}

static inline uint16_t get_reg_u16(cpu_t *cpu, uint8_t reg)
{
	reg <<= 1;

	return (cpu->regs[reg + 1] << 8) + cpu->regs[reg];
}

static inline void set_reg_u16(cpu_t *cpu, uint8_t reg, uint16_t val)
{
	reg <<= 1;

	cpu->regs[reg] = (uint8_t)(val & 0xff);
	++reg;
	cpu->regs[reg] = (uint8_t)(val >> 8);
}

// flag

static inline void set_flag(cpu_t *cpu, uint16_t flag)
{
	cpu->flags |= flag;
}

static inline uint8_t get_flag(cpu_t *cpu, uint16_t flag)
{
	return (uint8_t)((cpu->flags & flag) ? 1 : 0);
}

static inline void clear_flag(cpu_t *cpu, uint16_t flag)
{
	cpu->flags &= ~(flag);
}

static inline void set_pf(cpu_t *cpu, uint8_t res)
{
	if(tbl_pf[res] != 0)
		set_flag(cpu, F_PF);
	else
		clear_flag(cpu, F_PF);
}

static inline void set_cf_u8(cpu_t *cpu, uint16_t res)
{
	if((res & 0x100) != 0)
		set_flag(cpu, F_CF);
	else
		clear_flag(cpu, F_CF);
}

static inline void set_zf_u8(cpu_t *cpu, uint16_t res)
{
	if((res & 0xff) == 0)
		set_flag(cpu, F_ZF);
	else
		clear_flag(cpu, F_ZF);
}

static inline void set_sf_u8(cpu_t *cpu, uint16_t res)
{
	if((res & 0x80) != 0)
		set_flag(cpu, F_SF);
	else
		clear_flag(cpu, F_SF);
}

static inline void set_of_u8(cpu_t *cpu, uint8_t dst, uint8_t src, uint16_t res)
{
	if(((res ^ src) & (res ^ dst) & 0x80) != 0)
		set_flag(cpu, F_OF);
	else
		clear_flag(cpu, F_OF);
}

static inline void set_af_u8(cpu_t *cpu, uint8_t dst, uint8_t src, uint16_t res)
{
	if(((src ^ dst ^ res) & 0x10) != 0)
		set_flag(cpu, F_AF);
	else
		clear_flag(cpu, F_AF);
}

static inline void set_cf_u16(cpu_t *cpu, uint32_t res)
{
	if((res & 0x10000) != 0)
		set_flag(cpu, F_CF);
	else
		clear_flag(cpu, F_CF);
}

static inline void set_zf_u16(cpu_t *cpu, uint32_t res)
{
	if((res & 0xffff) == 0)
		set_flag(cpu, F_ZF);
	else
		clear_flag(cpu, F_ZF);
}

static inline void set_sf_u16(cpu_t *cpu, uint32_t res)
{
	if((res & 0x8000) != 0)
		set_flag(cpu, F_SF);
	else
		clear_flag(cpu, F_SF);
}

static inline void set_of_u16(cpu_t *cpu, uint16_t dst, uint16_t src, uint32_t res)
{
	if(((res ^ src) & (res ^ dst) & 0x8000) != 0)
		set_flag(cpu, F_OF);
	else
		clear_flag(cpu, F_OF);
}

static inline void set_af_u16(cpu_t *cpu, uint16_t dst, uint16_t src, uint32_t res)
{
	if(((src ^ dst ^ res) & 0x10) != 0)
		set_flag(cpu, F_AF);
	else
		clear_flag(cpu, F_AF);
}

// mem

static inline uint8_t get_mem_u8(cpu_t *cpu, uint16_t offt)
{
	return *(translate_x16_to_x64(DSEG, offt));
}

static inline void set_mem_u8(cpu_t *cpu, uint16_t offt, uint8_t val)
{
	uint8_t *dst = translate_x16_to_x64(DSEG, offt);
	*dst = val;
}

static inline uint16_t get_mem_u16(cpu_t *cpu, uint16_t offt)
{
	return get_mem_u8(cpu, offt) + (get_mem_u8(cpu, (uint16_t)(offt + 1)) << 8);
}

static inline void set_mem_u16(cpu_t *cpu, uint16_t offt, uint16_t val)
{
	set_mem_u8(cpu, offt, (uint8_t)(val & 0xff));
	++offt;
	set_mem_u8(cpu, offt, (uint8_t)(val >> 8));
}

// stack

static inline void push_u16(cpu_t *cpu, uint16_t val)
{
	register uint16_t sp = (uint16_t)(get_reg_u16(cpu, REG_SP) - 2);
	set_reg_u16(cpu, REG_SP, sp);

	uint16_t *dst = (uint16_t*)translate_x16_to_x64(DSEG, sp);
	*dst = val;
}

static inline uint16_t pop_u16(cpu_t *cpu)
{
	register uint16_t sp = get_reg_u16(cpu, REG_SP);
	set_reg_u16(cpu, REG_SP, (uint16_t)(sp + 2));

	uint16_t *val = (uint16_t*)translate_x16_to_x64(DSEG, sp);
	return *val;
}

// fetch

static inline uint8_t fetch_u8(cpu_t *cpu)
{
	return get_mem_u8(cpu, cpu->ip++);
}

static inline uint16_t fetch_u16(cpu_t *cpu)
{
	return fetch_u8(cpu) + (fetch_u8(cpu) << 8);
}

static modrm_t fetch_modrm(cpu_t *cpu)
{
	modrm_t modrm;

	register uint8_t code = fetch_u8(cpu);

	modrm.mod = (uint8_t)((code & 0xc0) >> 6);
	modrm.reg = (uint8_t)((code & 0x38) >> 3);
	modrm.rm = (uint8_t)(code & 0x07);

	return modrm;
}

// decode

static uint16_t decode_rm_addr(cpu_t *cpu, modrm_t *modrm)
{
	register uint16_t addr;

	assert(modrm->mod != 3);

	if(modrm->rm >> 1 == 3) {
		if(modrm->rm & 1)
			addr = get_reg_u16(cpu, REG_BX);
		else
			addr = modrm->mod ? get_reg_u16(cpu, REG_BP) : fetch_u16(cpu);
	} else {
		addr = get_reg_u16(cpu, REG_SI + (modrm->rm & 1));
		if(!(modrm->rm & 4))
			addr += get_reg_u16(cpu, REG_BX + (modrm->rm & 2));
	}

	switch(modrm->mod)
	{
		case 0x01:
			addr += (int8_t)fetch_u8(cpu);
			break;
		case 0x02:
			addr += fetch_u16(cpu);
			break;
		default:
			break;
	}

	return addr;
}

#include "opcodes.h"

cpu_t *cpu_new(pfn_far_call_cb callback)
{
	cpu_t *cpu = calloc(1, sizeof(cpu_t));
	if (cpu == NULL)
		goto exit;

	cpu->callback = callback;
	cpu_set_state(cpu, CPU_STOPPED);

exit:
	return cpu;
}

void cpu_reset(cpu_t *cpu, uint16_t addr)
{
	cpu->ip = addr;
	cpu->flags = 0x7202;
	set_reg_u16(cpu, REG_SP, STACK_OFFT + STACK_SIZE);
	cpu_set_state(cpu, CPU_RUNNING);
}

void cpu_set_state(cpu_t *cpu, uint8_t state)
{
	cpu->state = state;
}

void cpu_run(cpu_t *cpu)
{
	uint8_t opcode;

	assert(cpu->state != CPU_HALTED);

	while (cpu->state == CPU_RUNNING) {
		opcode = fetch_u8(cpu);
		if (tbl_opcodes[opcode] == NULL) {
			fprintf(stderr, "unknown opcode: %02x\n", opcode);
			assert(tbl_opcodes[opcode] != NULL);
		}

		(tbl_opcodes[opcode])(cpu, opcode);
		/* printf("op: %x; ax: %x; bx: %x; cx: %x; dx: %x; sp: %x; flags: %x\n",
			opcode, get_reg_u16(cpu, REG_AX), get_reg_u16(cpu, REG_BX), get_reg_u16(cpu, REG_CX),
			get_reg_u16(cpu, REG_DX), get_reg_u16(cpu, REG_SP), cpu->flags);
		*/
	}
}

void cpu_destroy(cpu_t *cpu)
{
	if (cpu)
		free(cpu);
}
