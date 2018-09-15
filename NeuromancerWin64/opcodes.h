#ifndef I8086_OPCODES_H
#define I8086_OPCODES_H

uint16_t add_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	register uint32_t res;

	res = dst + src;

	set_cf_u16(cpu, res);
	set_zf_u16(cpu, res);
	set_sf_u16(cpu, res);
	set_pf(cpu, (uint8_t)(res & 0xff));
	set_of_u16(cpu, dst, src, res);
	set_af_u16(cpu, dst, src, res);

	return (uint16_t)res;
}

uint16_t or_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	register uint16_t res;

	res = dst | src;

	clear_flag(cpu, F_CF);
	clear_flag(cpu, F_OF);
	clear_flag(cpu, F_AF);
	set_zf_u16(cpu, res);
	set_sf_u16(cpu, res);
	set_pf(cpu, (res & 0xff));

	return res;
}

uint16_t adc_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	register uint32_t res;

	res = dst + src + get_flag(cpu, F_CF);

	set_cf_u16(cpu, res);
	set_zf_u16(cpu, res);
	set_sf_u16(cpu, res);
	set_pf(cpu, (uint8_t)(res & 0xff));
	set_of_u16(cpu, dst, src, res);
	set_af_u16(cpu, dst, src, res);

	return (uint16_t)res;
}

uint16_t sbb_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	register uint32_t res;

	res = dst - src - get_flag(cpu, F_CF);

	set_cf_u16(cpu, res);
	set_zf_u16(cpu, res);
	set_sf_u16(cpu, res);
	set_pf(cpu, (uint8_t)(res & 0xff));
	set_of_u16(cpu, (uint16_t)(0xffff - dst), src, res);
	set_af_u16(cpu, dst, src, res);

	return (uint16_t)res;
}

uint16_t and_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	register uint16_t res;

	res = dst & src;

	clear_flag(cpu, F_CF);
	clear_flag(cpu, F_OF);
	clear_flag(cpu, F_AF);
	set_zf_u16(cpu, res);
	set_sf_u16(cpu, res);
	set_pf(cpu, (res & 0xff));

	return res;
}

uint16_t sub_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	register uint32_t res;

	res = dst - src;

	set_cf_u16(cpu, res);
	set_zf_u16(cpu, res);
	set_sf_u16(cpu, res);
	set_pf(cpu, (uint8_t)(res & 0xff));
	set_of_u16(cpu, (uint16_t)-dst, src, res);
	set_af_u16(cpu, dst, src, res);

	return (uint16_t)res;
}

uint16_t xor_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	register uint16_t res;

	res = dst ^ src;

	clear_flag(cpu, F_CF);
	clear_flag(cpu, F_OF);
	clear_flag(cpu, F_AF);
	set_zf_u16(cpu, res);
	set_sf_u16(cpu, res);
	set_pf(cpu, (res & 0xff));

	return res;
}

uint16_t cmp_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	sub_u16(cpu, dst, src);

	return (uint16_t)dst;
}

void test_u16(cpu_t *cpu, uint16_t dst, uint16_t src)
{
	register uint16_t res;

	res = dst & src;

	clear_flag(cpu, F_CF);
	clear_flag(cpu, F_OF);
	set_zf_u16(cpu, res);
	set_sf_u16(cpu, res);
	set_pf(cpu, (res & 0xff));
}

static uint16_t (*tbl_ops_u16[8])(cpu_t *cpu, uint16_t dst, uint16_t src) = {
		add_u16, or_u16, adc_u16, sbb_u16, and_u16, sub_u16, xor_u16, cmp_u16
};

uint8_t add_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	register uint16_t res;

	res = dst + src;

	set_cf_u8(cpu, res);
	set_zf_u8(cpu, res);
	set_sf_u8(cpu, res);
	set_pf(cpu, (uint8_t)(res & 0xff));
	set_of_u8(cpu, dst, src, res);
	set_af_u8(cpu, dst, src, res);

	return (uint8_t)res;
}

uint8_t or_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	register uint8_t res;

	res = dst | src;

	clear_flag(cpu, F_CF);
	clear_flag(cpu, F_OF);
	clear_flag(cpu, F_AF);
	set_zf_u8(cpu, res);
	set_sf_u8(cpu, res);
	set_pf(cpu, (res & 0xff));

	return res;
}

uint8_t adc_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	register uint16_t res;

	res = dst + src + get_flag(cpu, F_CF);

	set_cf_u8(cpu, res);
	set_zf_u8(cpu, res);
	set_sf_u8(cpu, res);
	set_pf(cpu, (uint8_t)(res & 0xff));
	set_of_u8(cpu, dst, src, res);
	set_af_u8(cpu, dst, src, res);

	return (uint8_t)res;
}

uint8_t sbb_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	register uint16_t res;

	res = dst - src - get_flag(cpu, F_CF);

	set_cf_u8(cpu, res);
	set_zf_u8(cpu, res);
	set_sf_u8(cpu, res);
	set_pf(cpu, (uint8_t)(res & 0xff));
	set_of_u8(cpu, (uint8_t)(0xff - dst), src, res);
	set_af_u8(cpu, dst, src, res);

	return (uint8_t)res;
}

uint8_t and_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	register uint8_t res;

	res = dst & src;

	clear_flag(cpu, F_CF);
	clear_flag(cpu, F_OF);
	clear_flag(cpu, F_AF);
	set_zf_u8(cpu, res);
	set_sf_u8(cpu, res);
	set_pf(cpu, (res & 0xff));

	return res;
}

uint8_t sub_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	register uint16_t res;

	res = dst - src;

	set_cf_u8(cpu, res);
	set_zf_u8(cpu, res);
	set_sf_u8(cpu, res);
	set_pf(cpu, (uint8_t)(res & 0xff));
	set_of_u8(cpu, (uint8_t)(0xff - dst), src, res);
	set_af_u8(cpu, dst, src, res);

	return (uint8_t)res;
}

uint8_t xor_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	register uint8_t res;

	res = dst ^ src;

	clear_flag(cpu, F_CF);
	clear_flag(cpu, F_OF);
	clear_flag(cpu, F_AF);
	set_zf_u8(cpu, res);
	set_sf_u8(cpu, res);
	set_pf(cpu, (res & 0xff));

	return res;
}

uint8_t cmp_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	sub_u16(cpu, dst, src);

	return (uint8_t)dst;
}

void test_u8(cpu_t *cpu, uint8_t dst, uint8_t src)
{
	register uint8_t res;

	res = dst & src;

	clear_flag(cpu, F_CF);
	clear_flag(cpu, F_OF);
	set_zf_u8(cpu, res);
	set_sf_u8(cpu, res);
	set_pf(cpu, (res & 0xff));
}

static uint8_t (*tbl_ops_u8[8])(cpu_t *cpu, uint8_t dst, uint8_t src) = {
		add_u8, or_u8, adc_u8, sbb_u8, and_u8, sub_u8, xor_u8, cmp_u8
};

static uint16_t check_condition(cpu_t *cpu, uint8_t opcode)
{
	uint16_t flags = cpu->flags;
	uint16_t res = 0;

	switch((opcode & 0x0f) >> 1) {
		case 0:	/* O */
			res = flags & F_OF;
			break;

		case 1:	/* B */
			res = flags & F_CF;
			break;

		case 3:	/* BE */
			res = (flags & F_CF) != 0;

		case 2:	/* Z */
			res |= (flags & F_ZF) != 0;
			break;

		case 4:	/* S */
			res = flags & F_SF;
			break;

		case 5:	/* P */
			res = flags & F_PF;
			break;

		case 7:	/* LE */
			res = (flags & F_ZF) != 0;

		case 6:	/* L */
			res |= ((flags & F_SF) != 0) ^ ((flags & F_OF) != 0);
			break;
	}

	return opcode & 1 ? !res : res;
}

static void opcode_00_cpuops_rm8_r8(cpu_t *cpu, uint8_t opcode)
{
	uint16_t addr;
	modrm_t modrm = fetch_modrm(cpu);

	if (modrm.mod == 3) {
		set_reg_u8(cpu, modrm.rm, (tbl_ops_u8[opcode >> 3])(cpu, get_reg_u8(cpu, modrm.rm), get_reg_u8(cpu, modrm.reg)));
	}
	else {
		addr = decode_rm_addr(cpu, &modrm);
		set_mem_u8(cpu, addr, (tbl_ops_u8[opcode >> 3])(cpu, get_mem_u8(cpu, addr), get_reg_u8(cpu, modrm.reg)));
	}
}

static void opcode_01_cpuops_rm16_r16(cpu_t *cpu, uint8_t opcode)
{
	uint16_t addr;
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3) {
		set_reg_u16(cpu, modrm.rm, (tbl_ops_u16[opcode >> 3])(cpu, get_reg_u16(cpu, modrm.rm), get_reg_u16(cpu, modrm.reg)));
	} else {
		addr = decode_rm_addr(cpu, &modrm);
		set_mem_u16(cpu, addr, (tbl_ops_u16[opcode >> 3])(cpu, get_mem_u16(cpu, addr), get_reg_u16(cpu, modrm.reg)));
	}
}

static void opcode_02_cpuops_r8_rm8(cpu_t *cpu, uint8_t opcode)
{
	uint16_t addr;
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3) {
		set_reg_u8(cpu, modrm.reg, (tbl_ops_u8[opcode >> 3])(cpu, get_reg_u8(cpu, modrm.reg), get_reg_u8(cpu, modrm.rm)));
	} else {
		addr = decode_rm_addr(cpu, &modrm);
		set_reg_u8(cpu, modrm.reg, (tbl_ops_u8[opcode >> 3])(cpu, get_reg_u8(cpu, modrm.reg), get_mem_u8(cpu, addr)));
	}
}

static void opcode_03_cpuops_r16_rm16(cpu_t *cpu, uint8_t opcode)
{
	uint16_t addr;
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3) {
		set_reg_u16(cpu, modrm.reg, (tbl_ops_u16[opcode >> 3])(cpu, get_reg_u16(cpu, modrm.reg), get_reg_u16(cpu, modrm.rm)));
	} else {
		addr = decode_rm_addr(cpu, &modrm);
		set_reg_u16(cpu, modrm.reg, (tbl_ops_u16[opcode >> 3])(cpu, get_reg_u16(cpu, modrm.reg), get_mem_u16(cpu, addr)));
	}
}

static void opcode_04_cpuops_al_imm8(cpu_t *cpu, uint8_t opcode)
{
	uint8_t imm8 = fetch_u8(cpu);

	set_reg_u8(cpu, REG_AL, (tbl_ops_u8[opcode >> 3])(cpu, get_reg_u8(cpu, REG_AL), imm8));
}

static void opcode_05_cpuops_ax_imm16(cpu_t *cpu, uint8_t opcode)
{
	uint16_t imm16 = fetch_u16(cpu);

	set_reg_u16(cpu, REG_AX, (tbl_ops_u16[opcode >> 3])(cpu, get_reg_u16(cpu, REG_AX), imm16));
}

static void opcode_40(cpu_t *cpu, uint8_t opcode)
{
	set_reg_u16(cpu, (uint8_t)(opcode & 0x07), add_u16(cpu, get_reg_u16(cpu, (uint8_t)(opcode & 0x07)), 1));
}

static void opcode_48(cpu_t *cpu, uint8_t opcode)
{
	set_reg_u16(cpu, (uint8_t)(opcode & 0x07), sub_u16(cpu, get_reg_u16(cpu, (uint8_t)(opcode & 0x07)), 1));
}

static void opcode_50(cpu_t *cpu, uint8_t opcode)
{
	push_u16(cpu, get_reg_u16(cpu, opcode & 0x07));
}

static void opcode_58(cpu_t *cpu, uint8_t opcode)
{
	set_reg_u16(cpu, opcode & 0x07, pop_u16(cpu));
}

static void opcode_70_jmps_cc(cpu_t *cpu, uint8_t opcode)
{
	int8_t offt = fetch_u8(cpu);

	if(check_condition(cpu, opcode))
		cpu->ip += offt;
}
static void opcode_81_cpuops_rm16_imm16(cpu_t *cpu, uint8_t opcode)
{
	modrm_t modrm = fetch_modrm(cpu);
	uint16_t addr = modrm.mod != 3 ? decode_rm_addr(cpu, &modrm) : 0;
	uint16_t imm16 = fetch_u16(cpu);

	if(modrm.mod == 3)
		set_reg_u16(cpu, modrm.rm, (tbl_ops_u16[modrm.reg])(cpu, get_reg_u16(cpu, modrm.rm), imm16));
	else
		set_mem_u16(cpu, addr, (tbl_ops_u16[modrm.reg])(cpu, get_mem_u16(cpu, addr), imm16));
}

static void opcode_83_cpuops_rm16_imm8(cpu_t *cpu, uint8_t opcode)
{
	modrm_t modrm = fetch_modrm(cpu);
	uint16_t addr = modrm.mod != 3 ? decode_rm_addr(cpu, &modrm) : 0;
	uint8_t imm8 = fetch_u8(cpu);

	if(modrm.mod == 3)
		set_reg_u16(cpu, modrm.rm, (tbl_ops_u16[modrm.reg])(cpu, get_reg_u16(cpu, modrm.rm), imm8));
	else
		set_mem_u16(cpu, addr, (tbl_ops_u16[modrm.reg])(cpu, get_mem_u16(cpu, addr), imm8));
}

static void opcode_84_test_rm8_r8(cpu_t *cpu, uint8_t opcode)
{
	uint16_t addr;
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3) {
		test_u8(cpu, get_reg_u8(cpu, modrm.rm), get_reg_u8(cpu, modrm.reg));
	} else {
		addr = decode_rm_addr(cpu, &modrm);
		test_u8(cpu, get_mem_u8(cpu, addr), get_reg_u8(cpu, modrm.reg));
	}
}

static void opcode_85_test_rm16_r16(cpu_t *cpu, uint8_t opcode)
{
	uint16_t addr;
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3) {
		test_u16(cpu, get_reg_u16(cpu, modrm.rm), get_reg_u16(cpu, modrm.reg));
	} else {
		addr = decode_rm_addr(cpu, &modrm);
		test_u16(cpu, get_mem_u16(cpu, addr), get_reg_u16(cpu, modrm.reg));
	}
}

static void opcode_88(cpu_t *cpu, uint8_t opcode)
{
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3)
		set_reg_u8(cpu, modrm.rm, get_reg_u8(cpu, modrm.reg));
	else
		set_mem_u8(cpu, decode_rm_addr(cpu, &modrm), get_reg_u8(cpu, modrm.reg));
}

static void opcode_89(cpu_t *cpu, uint8_t opcode)
{
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3)
		set_reg_u16(cpu, modrm.rm, get_reg_u16(cpu, modrm.reg));
	else
		set_mem_u16(cpu, decode_rm_addr(cpu, &modrm), get_reg_u16(cpu, modrm.reg));
}

static void opcode_8a(cpu_t *cpu, uint8_t opcode)
{
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3)
		set_reg_u8(cpu, modrm.reg, get_reg_u8(cpu, modrm.rm));
	else
		set_reg_u8(cpu, modrm.reg, get_mem_u8(cpu, decode_rm_addr(cpu, &modrm)));
}

static void opcode_8b(cpu_t *cpu, uint8_t opcode)
{
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3)
		set_reg_u16(cpu, modrm.reg, get_reg_u16(cpu, modrm.rm));
	else
		set_reg_u16(cpu, modrm.reg, get_mem_u16(cpu, decode_rm_addr(cpu, &modrm)));
}

static void opcode_b0(cpu_t *cpu, uint8_t opcode)
{
	set_reg_u8(cpu, opcode & 0x07, fetch_u8(cpu));
}

static void opcode_b8(cpu_t *cpu, uint8_t opcode)
{
	set_reg_u16(cpu, opcode & 0x07, fetch_u16(cpu));
}

static void opcode_c3(cpu_t *cpu, uint8_t opcode)
{
	cpu->ip = pop_u16(cpu);
}

static void opcode_c6_mov_rm8_imm8(cpu_t *cpu, uint8_t opcode)
{
	modrm_t modrm = fetch_modrm(cpu);
	uint16_t addr = modrm.mod != 3 ? decode_rm_addr(cpu, &modrm) : 0;
	uint8_t imm8 = fetch_u8(cpu);

	assert(modrm.reg == 0);

	if(modrm.mod == 3) {
		set_reg_u8(cpu, modrm.rm, imm8);
	} else {
		set_mem_u8(cpu, addr, imm8);
	}
}

static void opcode_c7_mov_rm16_imm16(cpu_t *cpu, uint8_t opcode)
{
	modrm_t modrm = fetch_modrm(cpu);
	uint16_t addr = modrm.mod != 3 ? decode_rm_addr(cpu, &modrm) : 0;
	uint16_t imm16 = fetch_u16(cpu);

	assert(modrm.reg == 0);

	if(modrm.mod == 3)
		set_reg_u16(cpu, modrm.rm, imm16);
	else
		set_mem_u16(cpu, addr, imm16);
}

static void opcode_cb(cpu_t *cpu, uint8_t opcode)
{
	cpu->state = CPU_HALTED;
}

static void opcode_d7(cpu_t *cpu, uint8_t opcode)
{
	set_reg_u8(cpu, REG_AL, get_mem_u8(cpu, get_reg_u16(cpu, REG_BX) + get_reg_u8(cpu, REG_AL)));
}

static void opcode_e2(cpu_t *cpu, uint8_t opcode)
{
	int8_t offt = fetch_u8(cpu);
	uint16_t cnt = get_reg_u16(cpu, REG_CX);

	set_reg_u16(cpu, REG_CX, --cnt);
	if(cnt)
		cpu->ip += offt;
}

static void opcode_e8(cpu_t *cpu, uint8_t opcode)
{
	int16_t offt = fetch_u16(cpu);
	uint16_t ip = cpu->ip + offt;

	push_u16(cpu, cpu->ip);
	cpu->ip = ip;
}

static void opcode_eb(cpu_t *cpu, uint8_t opcode)
{
	int8_t offt = fetch_u8(cpu);

	cpu->ip += offt;
}

static void opcode_fe(cpu_t *cpu, uint8_t opcode)
{
	uint16_t addr;
	modrm_t modrm = fetch_modrm(cpu);

	if(modrm.mod == 3) {
		set_reg_u8(cpu, modrm.rm, (modrm.reg ? sub_u8 : add_u8)(cpu, get_reg_u8(cpu, modrm.rm), 1));
	} else {
		addr = decode_rm_addr(cpu, &modrm);
		set_mem_u8(cpu, addr, (modrm.reg ? sub_u8 : add_u8)(cpu, get_mem_u8(cpu, addr), 1));
	}
}

static void opcode_ff(cpu_t *cpu, uint8_t opcode)
{
	uint16_t addr;
	modrm_t modrm = fetch_modrm(cpu);

	switch(modrm.reg) {
		case 0:
		case 1:
			if(modrm.mod == 3) {
				set_reg_u16(cpu, modrm.rm, (modrm.reg ? sub_u16 : add_u16)(cpu, get_reg_u16(cpu, modrm.rm), 1));
			} else {
				addr = decode_rm_addr(cpu, &modrm);
				set_mem_u16(cpu, addr, (modrm.reg ? sub_u16 : add_u16)(cpu, get_mem_u16(cpu, addr), 1));
			}
			break;
		case 3:
			assert(modrm.mod != 3);

			addr = decode_rm_addr(cpu, &modrm);
			push_u16(cpu, 0);
			push_u16(cpu, cpu->ip);
			cpu->state = (cpu->callback)(get_reg_u16(cpu, REG_SP));
			pop_u16(cpu);
			pop_u16(cpu);
			break;
		case 6:
			if(modrm.mod == 3) {
				push_u16(cpu, get_reg_u16(cpu, modrm.rm));
			} else {
				addr = decode_rm_addr(cpu, &modrm);
				push_u16(cpu, get_mem_u16(cpu, addr));
			}
			break;
	}
}

void (*tbl_opcodes[256])(cpu_t *cpu, uint8_t opcode) =
{
	/* 0x00 */ &opcode_00_cpuops_rm8_r8,
	/* 0x01 */ &opcode_01_cpuops_rm16_r16,
	/* 0x02 */ &opcode_02_cpuops_r8_rm8,
	/* 0x03 */ &opcode_03_cpuops_r16_rm16,
	/* 0x04 */ &opcode_04_cpuops_al_imm8,
	/* 0x05 */ &opcode_05_cpuops_ax_imm16,
	/* 0x06 */ NULL,
	/* 0x07 */ NULL,
	/* 0x08 */ &opcode_00_cpuops_rm8_r8,
	/* 0x09 */ &opcode_01_cpuops_rm16_r16,
	/* 0x0a */ &opcode_02_cpuops_r8_rm8,
	/* 0x0b */ &opcode_03_cpuops_r16_rm16,
	/* 0x0c */ &opcode_04_cpuops_al_imm8,
	/* 0x0d */ &opcode_05_cpuops_ax_imm16,
	/* 0x0e */ NULL,
	/* 0x0f */ NULL,
	/* 0x10 */ &opcode_00_cpuops_rm8_r8,
	/* 0x11 */ &opcode_01_cpuops_rm16_r16,
	/* 0x12 */ &opcode_02_cpuops_r8_rm8,
	/* 0x13 */ &opcode_03_cpuops_r16_rm16,
	/* 0x14 */ &opcode_04_cpuops_al_imm8,
	/* 0x15 */ &opcode_05_cpuops_ax_imm16,
	/* 0x16 */ NULL,
	/* 0x17 */ NULL,
	/* 0x18 */ &opcode_00_cpuops_rm8_r8,
	/* 0x19 */ &opcode_01_cpuops_rm16_r16,
	/* 0x1a */ &opcode_02_cpuops_r8_rm8,
	/* 0x1b */ &opcode_03_cpuops_r16_rm16,
	/* 0x1c */ &opcode_04_cpuops_al_imm8,
	/* 0x1d */ &opcode_05_cpuops_ax_imm16,
	/* 0x1e */ NULL,
	/* 0x1f */ NULL,
	/* 0x20 */ &opcode_00_cpuops_rm8_r8,
	/* 0x21 */ &opcode_01_cpuops_rm16_r16,
	/* 0x22 */ &opcode_02_cpuops_r8_rm8,
	/* 0x23 */ &opcode_03_cpuops_r16_rm16,
	/* 0x24 */ &opcode_04_cpuops_al_imm8,
	/* 0x25 */ &opcode_05_cpuops_ax_imm16,
	/* 0x26 */ NULL,
	/* 0x27 */ NULL,
	/* 0x28 */ &opcode_00_cpuops_rm8_r8,
	/* 0x29 */ &opcode_01_cpuops_rm16_r16,
	/* 0x2a */ &opcode_02_cpuops_r8_rm8,
	/* 0x2b */ &opcode_03_cpuops_r16_rm16,
	/* 0x2c */ &opcode_04_cpuops_al_imm8,
	/* 0x2d */ &opcode_05_cpuops_ax_imm16,
	/* 0x2e */ NULL,
	/* 0x2f */ NULL,
	/* 0x30 */ &opcode_00_cpuops_rm8_r8,
	/* 0x31 */ &opcode_01_cpuops_rm16_r16,
	/* 0x32 */ &opcode_02_cpuops_r8_rm8,
	/* 0x33 */ &opcode_03_cpuops_r16_rm16,
	/* 0x34 */ &opcode_04_cpuops_al_imm8,
	/* 0x35 */ &opcode_05_cpuops_ax_imm16,
	/* 0x36 */ NULL,
	/* 0x37 */ NULL,
	/* 0x38 */ &opcode_00_cpuops_rm8_r8,
	/* 0x39 */ &opcode_01_cpuops_rm16_r16,
	/* 0x3a */ &opcode_02_cpuops_r8_rm8,
	/* 0x3b */ &opcode_03_cpuops_r16_rm16,
	/* 0x3c */ &opcode_04_cpuops_al_imm8,
	/* 0x3d */ &opcode_05_cpuops_ax_imm16,
	/* 0x3e */ NULL,
	/* 0x3f */ NULL,
	/* 0x40 */ &opcode_40,
	/* 0x41 */ &opcode_40,
	/* 0x42 */ &opcode_40,
	/* 0x43 */ &opcode_40,
	/* 0x44 */ &opcode_40,
	/* 0x45 */ &opcode_40,
	/* 0x46 */ &opcode_40,
	/* 0x47 */ &opcode_40,
	/* 0x48 */ &opcode_48,
	/* 0x49 */ &opcode_48,
	/* 0x4a */ &opcode_48,
	/* 0x4b */ &opcode_48,
	/* 0x4c */ &opcode_48,
	/* 0x4d */ &opcode_48,
	/* 0x4e */ &opcode_48,
	/* 0x4f */ &opcode_48,
	/* 0x50 */ &opcode_50,
	/* 0x51 */ &opcode_50,
	/* 0x52 */ &opcode_50,
	/* 0x53 */ &opcode_50,
	/* 0x54 */ &opcode_50,
	/* 0x55 */ &opcode_50,
	/* 0x56 */ &opcode_50,
	/* 0x57 */ &opcode_50,
	/* 0x58 */ &opcode_58,
	/* 0x59 */ &opcode_58,
	/* 0x5a */ &opcode_58,
	/* 0x5b */ &opcode_58,
	/* 0x5c */ &opcode_58,
	/* 0x5d */ &opcode_58,
	/* 0x5e */ &opcode_58,
	/* 0x5f */ &opcode_58,
	/* 0x60 */ NULL,
	/* 0x61 */ NULL,
	/* 0x62 */ NULL,
	/* 0x63 */ NULL,
	/* 0x64 */ NULL,
	/* 0x65 */ NULL,
	/* 0x66 */ NULL,
	/* 0x67 */ NULL,
	/* 0x68 */ NULL,
	/* 0x69 */ NULL,
	/* 0x6a */ NULL,
	/* 0x6b */ NULL,
	/* 0x6c */ NULL,
	/* 0x6d */ NULL,
	/* 0x6e */ NULL,
	/* 0x6f */ NULL,
	/* 0x70 */ &opcode_70_jmps_cc,
	/* 0x71 */ &opcode_70_jmps_cc,
	/* 0x72 */ &opcode_70_jmps_cc,
	/* 0x73 */ &opcode_70_jmps_cc,
	/* 0x74 */ &opcode_70_jmps_cc,
	/* 0x75 */ &opcode_70_jmps_cc,
	/* 0x76 */ &opcode_70_jmps_cc,
	/* 0x77 */ &opcode_70_jmps_cc,
	/* 0x78 */ &opcode_70_jmps_cc,
	/* 0x79 */ &opcode_70_jmps_cc,
	/* 0x7a */ &opcode_70_jmps_cc,
	/* 0x7b */ &opcode_70_jmps_cc,
	/* 0x7c */ &opcode_70_jmps_cc,
	/* 0x7d */ &opcode_70_jmps_cc,
	/* 0x7e */ &opcode_70_jmps_cc,
	/* 0x7f */ &opcode_70_jmps_cc,
	/* 0x80 */ NULL,
	/* 0x81 */ &opcode_81_cpuops_rm16_imm16,
	/* 0x82 */ NULL,
	/* 0x83 */ &opcode_83_cpuops_rm16_imm8,
	/* 0x84 */ &opcode_84_test_rm8_r8,
	/* 0x85 */ &opcode_85_test_rm16_r16,
	/* 0x86 */ NULL,
	/* 0x87 */ NULL,
	/* 0x88 */ &opcode_88,
	/* 0x89 */ &opcode_89,
	/* 0x8a */ &opcode_8a,
	/* 0x8b */ &opcode_8b,
	/* 0x8c */ NULL,
	/* 0x8d */ NULL,
	/* 0x8e */ NULL,
	/* 0x8f */ NULL,
	/* 0x90 */ NULL,
	/* 0x91 */ NULL,
	/* 0x92 */ NULL,
	/* 0x93 */ NULL,
	/* 0x94 */ NULL,
	/* 0x95 */ NULL,
	/* 0x96 */ NULL,
	/* 0x97 */ NULL,
	/* 0x98 */ NULL,
	/* 0x99 */ NULL,
	/* 0x9a */ NULL,
	/* 0x9b */ NULL,
	/* 0x9c */ NULL,
	/* 0x9d */ NULL,
	/* 0x9e */ NULL,
	/* 0x9f */ NULL,
	/* 0xa0 */ NULL,
	/* 0xa1 */ NULL,
	/* 0xa2 */ NULL,
	/* 0xa3 */ NULL,
	/* 0xa4 */ NULL,
	/* 0xa5 */ NULL,
	/* 0xa6 */ NULL,
	/* 0xa7 */ NULL,
	/* 0xa8 */ NULL,
	/* 0xa9 */ NULL,
	/* 0xaa */ NULL,
	/* 0xab */ NULL,
	/* 0xac */ NULL,
	/* 0xad */ NULL,
	/* 0xae */ NULL,
	/* 0xaf */ NULL,
	/* 0xb0 */ &opcode_b0,
	/* 0xb1 */ &opcode_b0,
	/* 0xb2 */ &opcode_b0,
	/* 0xb3 */ &opcode_b0,
	/* 0xb4 */ &opcode_b0,
	/* 0xb5 */ &opcode_b0,
	/* 0xb6 */ &opcode_b0,
	/* 0xb7 */ &opcode_b0,
	/* 0xb8 */ &opcode_b8,
	/* 0xb9 */ &opcode_b8,
	/* 0xba */ &opcode_b8,
	/* 0xbb */ &opcode_b8,
	/* 0xbc */ &opcode_b8,
	/* 0xbd */ &opcode_b8,
	/* 0xbe */ &opcode_b8,
	/* 0xbf */ &opcode_b8,
	/* 0xc0 */ NULL,
	/* 0xc1 */ NULL,
	/* 0xc2 */ NULL,
	/* 0xc3 */ &opcode_c3,
	/* 0xc4 */ NULL,
	/* 0xc5 */ NULL,
	/* 0xc6 */ &opcode_c6_mov_rm8_imm8,
	/* 0xc7 */ &opcode_c7_mov_rm16_imm16,
	/* 0xc8 */ NULL,
	/* 0xc9 */ NULL,
	/* 0xca */ NULL,
	/* 0xcb */ &opcode_cb,
	/* 0xcc */ NULL,
	/* 0xcd */ NULL,
	/* 0xce */ NULL,
	/* 0xcf */ NULL,
	/* 0xd0 */ NULL,
	/* 0xd1 */ NULL,
	/* 0xd2 */ NULL,
	/* 0xd3 */ NULL,
	/* 0xd4 */ NULL,
	/* 0xd5 */ NULL,
	/* 0xd6 */ NULL,
	/* 0xd7 */ &opcode_d7,
	/* 0xd8 */ NULL,
	/* 0xd9 */ NULL,
	/* 0xda */ NULL,
	/* 0xdb */ NULL,
	/* 0xdc */ NULL,
	/* 0xdd */ NULL,
	/* 0xde */ NULL,
	/* 0xdf */ NULL,
	/* 0xe0 */ NULL,
	/* 0xe1 */ NULL,
	/* 0xe2 */ &opcode_e2,
	/* 0xe3 */ NULL,
	/* 0xe4 */ NULL,
	/* 0xe5 */ NULL,
	/* 0xe6 */ NULL,
	/* 0xe7 */ NULL,
	/* 0xe8 */ &opcode_e8,
	/* 0xe9 */ NULL,
	/* 0xea */ NULL,
	/* 0xeb */ &opcode_eb,
	/* 0xec */ NULL,
	/* 0xed */ NULL,
	/* 0xee */ NULL,
	/* 0xef */ NULL,
	/* 0xf0 */ NULL,
	/* 0xf1 */ NULL,
	/* 0xf2 */ NULL,
	/* 0xf3 */ NULL,
	/* 0xf4 */ NULL,
	/* 0xf5 */ NULL,
	/* 0xf6 */ NULL,
	/* 0xf7 */ NULL,
	/* 0xf8 */ NULL,
	/* 0xf9 */ NULL,
	/* 0xfa */ NULL,
	/* 0xfb */ NULL,
	/* 0xfc */ NULL,
	/* 0xfd */ NULL,
	/* 0xfe */ &opcode_fe,
	/* 0xff */ &opcode_ff
};

#endif
