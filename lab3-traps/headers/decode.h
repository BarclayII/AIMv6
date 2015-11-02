/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_DECODE_H
#define _ASM_DECODE_H

/* The first 6-bit field is always opcode */
#define OPCODE(insn)		(((insn) >> 26) & 0x3f)

/*
 * MIPS instructions have different types:
 */

/*
 * R-instructions, generally for three-register operations.
 * (Each minus sign '-' represent one bit)
 * +------+-----+-----+-----+-----+------+
 * |opcode| r s | r t | r d |shift| func |
 * +------+-----+-----+-----+-----+------+
 *
 * Some R-instructions, namely BREAK and SYSCALL, combine 4 5-bit fields into
 * a 20-bit code field.  We denote them RC-instructions:
 * +------+--------------------+------+
 * |opcode|        code        | func |
 * +------+--------------------+------+
 *
 * Coprocessor 0 instructions, having opcode OP_COP0, reuse rs field as a
 * sub-operation field, and put operands into function field.  Maybe I can
 * call them C0-instructions?
 * The operand may be a register selector or a function number.
 * +------+-----+-----+-----+-----+------+
 * |opcode|subop| r t | r d |  0  |oprand|
 * +------+-----+-----+-----+-----+------+
 *
 * Shifting instructions (S-instructions) is a subtype of R-instructions.
 *
 * Fixed shifting instructions (being denoted by SF-instructions here) reuse
 * rs field to indicate whether the processor should execute a logical rotation
 * or a shift.
 * +------+-----+-----+-----+-----+------+
 * |opcode| rot | r t | r d |shift| func |
 * +------+-----+-----+-----+-----+------+
 *
 * Variable shifting instructions (called SV-instructions here), which shifts
 * registers for a variable number of bits, reuses shift field instead for
 * rotation indication.
 * +------+-----+-----+-----+-----+------+
 * |opcode| r s | r t | r d | rot | func |
 * +------+-----+-----+-----+-----+------+
 *
 * Some instructions having opcode OP_SPECIAL3 have weird encodings.
 *
 * The first subset of SPECIAL3 instructions deals with bit field extraction
 * or insertion.  Perhaps we can call them XI-instructions.
 * The rd field is replaced by a field keeping the most significant bit (MSB),
 * or sometimes MSB minus 32, according to function field.
 * The shift field is also replaced by a least significant bit (LSB) field.
 * Again, with different function field it may also store LSB minus 32.
 * +------+-----+-----+-----+-----+------+
 * |opcode| r s | r t | msb | lsb | func |
 * +------+-----+-----+-----+-----+------+
 *
 * Another subset of SPECIAL3 instructions, here named X-instructions, swaps
 * or reverses bytes, halfwords, or words in a halfword, word or doubleword,
 * or sign-extends bytes and halfwords into longer units.
 * The actual operation is determined by a suboperation field which takes
 * place of shift field.
 * I named them X-instructions because I don't really know how I should name
 * them.
 * +------+-----+-----+-----+-----+------+
 * |opcode|  0  | r t | r d |subop| func |
 * +------+-----+-----+-----+-----+------+
 */
#define R_RS(insn)		(((insn) >> 21) & 0x1f)
#define R_RT(insn)		(((insn) >> 16) & 0x1f)
#define R_RD(insn)		(((insn) >> 11) & 0x1f)
#define R_SHIFT(insn)		(((insn) >>  6) & 0x1f)
#define R_FUNC(insn)		(((insn)      ) & 0x3f)
#define R_CODE(insn)		(((insn) >>  6) & 0xfffff)
#define R_SUBOP(insn)		R_RS(insn)
#define R_OPERAND(insn)		R_FUNC(insn)
#define SF_ROT(insn)		R_RS(insn)
#define SV_ROT(insn)		R_SHIFT(insn)

/*
 * I-instructions, mainly for two-register plus one-immediate operations.
 * Branch instructions (Bxx) are mostly I-instructions, so the actual
 * offset is 4 times immediate value.
 *
 * +------+-----+-----+----------------+
 * |opcode| r s | r t |immediate/offset|
 * +------+-----+-----+----------------+
 *
 * A subtype of I-instruction put function field like R-instructions in rt
 * field.  These instructions have opcode OP_REGIMM(0b000001).  We can call
 * them RI-instructions.
 *
 * +------+-----+-----+----------------+
 * |opcode| r s |funct|immediate/offset|
 * +------+-----+-----+----------------+
 */
#define I_RS(insn)		(((insn) >> 21) & 0x1f)
#define I_RT(insn)		(((insn) >> 16) & 0x1f)
#define I_IMMEDIATE(insn)	(((insn)      ) & 0xffff)
#define I_OFFSET(insn)		((signed short)(I_IMMEDIATE(insn)))
#define I_IOFFSET(insn)		(((unsigned long)(I_OFFSET(insn))) << 2)
#define I_FUNC(insn)		I_RT(insn)

/*
 * J-instructions for (relatively far) jumps.
 * Note that actual index is OFFSET(insn) << 2 since instructions are always
 * 4 bytes long.
 * The lowest 28 bits of current program counter is replaced by actual index
 * after executing the jump instruction.
 *
 * +------+--------------------------+
 * |opcode|          index           |
 * +------+--------------------------+
 */
#define J_INDEX_BITS		26
#define J_INDEX_MASK		((1 << J_INDEX_BITS) - 1)
#define J_ACTUAL_INDEX_BITS	(J_INDEX_BITS + 2)
#define J_ACTUAL_INDEX_MASK	((1 << J_ACTUAL_INDEX_BITS) - 1)
#define J_INDEX_PARAM(insn)	(((insn)      ) & J_INDEX_MASK)
#define J_INDEX(insn)		((((insn)     ) & J_INDEX_MASK) << 2)

/*
 * There are also FP- and FI-instructions for floating point arithmetic but
 * currently I'm not adding them here yet.
 *
 * Coprocessor 2 operations are also ignored at present.
 */

/* Opcode field and function field values */
#define OP_SPECIAL	0x00	/* R-instructions */
# define FN_SLL		 0x00	/* (SF) sll: Shift Left Logical */
/* 			 0x01 */
# define FN_SRL		 0x02	/* (SF) srl or rotr: Shift/ROTate Right */
# define FN_SRA		 0x03	/* (SF) sra: Shift Right Arithmetic */
# define FN_SLLV	 0x04	/* (SV) sllv: SLL Variable */
/* 			 0x05 */
# define FN_SRLV	 0x06	/* (SV) srlv or rotrv: SRL/ROTR Variable */
# define FN_SRAV	 0x07	/* (SV) srav: SRA Variable */
# define FN_JR		 0x08	/* (R)  jr: Jump Register */
# define FN_JALR	 0x09	/* (R)  jalr: Jump And Link Register */
# define FN_MOVZ	 0x0a	/* (R)  movz: MOVe on Zero */
# define FN_MOVN	 0x0b	/* (R)  movn: MOVe on Nonzero */
# define FN_SYSCALL	 0x0c	/* (RC) syscall: SYStem CALL */
# define FN_BREAK	 0x0d	/* (RC) break: Breakpoint */
/*			 0x0e */
# define FN_SYNC	 0x0f	/* (R)  sync: SYNChronize shared memory */
# define FN_MFHI	 0x10	/* (R)  mfhi: Move From HI */
# define FN_MTHI	 0x11	/* (R)  mthi: Move To HI */
# define FN_MFLO	 0x12	/* (R)  mflo: Move From LO */
# define FN_MTLO	 0x13	/* (R)  mtlo: Move TO LO */
# define FN_DSLLV	 0x14	/* (SV) dsllv: DSLL Variable */
/*			 0x15 */
# define FN_DSRLV	 0x16	/* (SV) dsrlv or drotrv: DSRL/DROTR Variable */
# define FN_DSRAV	 0x17	/* (SV) dsrav: DSRA Variable */
# define FN_MULT	 0x18	/* (R)  mult: MULTiply (to HI-LO) */
# define FN_MULTU	 0x19	/* (R)  multu: MULT Unsigned (to HI-LO) */
# define FN_DIV		 0x1a	/* (R)  div: DIVide */
# define FN_DIVU	 0x1b	/* (R)  divu: DIVide Unsigned */
# define FN_DMULT	 0x1c	/* (R)  dmult: Doubleword MULT */
# define FN_DMULTU	 0x1d	/* (R)  dmultu: Doubleword MULTU */
# define FN_DDIV	 0x1e	/* (R)  ddiv: Doubleword DIV */
# define FN_DDIVU	 0x1f	/* (R)  ddivu: Doubleword DIVU */
# define FN_ADD		 0x20	/* (R)  add: ADD registers */
# define FN_ADDU	 0x21	/* (R)  addu: ADD Unsigned registers */
# define FN_SUB		 0x22	/* (R)  sub: SUBtract registers */
# define FN_SUBU	 0x23	/* (R)  subu: SUBtract Unsigned registers */
# define FN_AND		 0x24	/* (R)  and: AND registers */
# define FN_OR		 0x25	/* (R)  or: OR registers */
# define FN_XOR		 0x26	/* (R)  xor: XOR registers */
# define FN_NOR		 0x27	/* (R)  nor: Not OR registers */
/*			 0x28 */
/*			 0x29 */
# define FN_SLT		 0x2a	/* (R)  slt: Set on Less Than */
# define FN_SLTU	 0x2b	/* (R)  sltu: Set on Less Than Unsigned */
# define FN_DADD	 0x2c	/* (R)  dadd: Doubleword ADD */
# define FN_DADDU	 0x2d	/* (R)  daddu: Doubleword ADDU */
# define FN_DSUB	 0x2e	/* (R)  dsub: Doubleword SUB */
# define FN_DSUBU	 0x2f	/* (R)  dsubu: Doubleword SUBU */
# define FN_TGE		 0x30	/* (R)  tge: Trap if Greater or Equal */
# define FN_TGEU	 0x31	/* (R)  tgeu: TGE Unsigned */
# define FN_TLT		 0x32	/* (R)  tlt: Trap if Less Than */
# define FN_TLTU	 0x33	/* (R)  tltu: TLT Unsigned */
# define FN_TEQ		 0x34	/* (R)  teq: Trap if EQual */
/*			 0x35 */
# define FN_TNE		 0x36	/* (R)  tne: Trap if Not Equal */
/*			 0x37 */
# define FN_DSLL	 0x38	/* (SF) dsll: Doubleword SLL */
/*			 0x39 */
# define FN_DSRL	 0x3a	/* (SF) dsrl or drotr: Doubleword SRL or ROTR */
# define FN_DSRA	 0x3b	/* (SF) dsra: Doubleword SRA */
# define FN_DSLL32	 0x3c	/* (SF) dsll32: DSLL + 32 */
/*			 0x3d */
# define FN_DSRL32	 0x3e	/* (SF) dsrl32 or drotr32: DSRL or DROTR + 32 */
# define FN_DSRA32	 0x3f	/* (SF) dsra32: Doubleword SRA + 32 */
#define OP_REGIMM	0x01	/* RI-instruction */
# define FN_BLTZ	 0x00	/* (RI) bltz: Branch if < 0 */
# define FN_BGEZ	 0x01	/* (RI) bgez: Branch if >= 0 */
# define FN_BLTZL	 0x02	/* (RI) bltzl: BLTZ Likely */
# define FN_BGEZL	 0x03	/* (RI) bgezl: BGEZ Likely */
/*			 0x04-0x07 */
# define FN_TGEI	 0x08	/* (RI) tgei: TGE Immediate */
# define FN_TGEIU	 0x09	/* (RI) tgeiu: TGE Immediate Unsigned */
# define FN_TLTI	 0x0a	/* (RI) tlti: TLT Immediate */
# define FN_TLTIU	 0x0b	/* (RI) tltiu: TLT Immediate Unsigned */
# define FN_TEQI	 0x0c	/* (RI) teqi: TEQ Immediate */
/*			 0x0d */
# define FN_TNEI	 0x0e	/* (RI) tnei: TNE Immediate */
/*			 0x0f */
# define FN_BLTZAL	 0x10	/* (RI) bltzal: Branch And Link if < 0 */
# define FN_BGEZAL	 0x11	/* (RI) bgezal: Branch And Link if >= 0 */
# define FN_BLTZALL	 0x12	/* (RI) bltzall: BLTZAL Likely */
# define FN_BGEZALL	 0x13	/* (RI) bgezall: BGEZAL Likely */
/*			 0x14-0x1e */
# define FN_SYNCI	 0x1f	/* (RI) synci: SYNChronize Instruction */
#define OP_J		0x02	/* (J)	j: Jump */
#define OP_JAL		0x03	/* (J)	jal: Jump And Link */
#define OP_BEQ		0x04	/* (I)  beq: Branch if EQual */
#define OP_BNE		0x05	/* (I)  bne: Branch if Not Equal */
#define OP_BLEZ		0x06	/* (I)  blez: Branch if <= 0 */
#define OP_BGTZ		0x07	/* (I)  bgtz: Branch if > 0 */
#define OP_ADDI		0x08	/* (I)  addi: ADD Immediate */
#define OP_ADDIU	0x09	/* (I)  addiu: ADD Immediate Unsigned */
#define OP_SLTI		0x0a	/* (I)  slti: Set on Less Than Immediate */
#define OP_SLTIU	0x0b	/* (I)  sltiu: SLTI Unsigned */
#define OP_ANDI		0x0c	/* (I)  andi: AND Immediate */
#define OP_ORI		0x0d	/* (I)  ori: OR Immediate */
#define OP_XORI		0x0e	/* (I)  xori: XOR Immediate */
#define OP_LUI		0x0f	/* (I)  lui: Load Upper Immediate */
#define OP_COP0		0x10	/* Coprocessor 0 instructions */
# define SO_MF		 0x00	/* (C0) mfc0: Move From CP0 */
# define SO_DMF		 0x01	/* (C0) dmfc0: Doubleword Move From CP0 */
/*			 0x02 */
/*			 0x03 */
# define SO_MT		 0x04	/* (C0) mtc0: Move To CP0 */
# define SO_DMT		 0x05	/* (C0) dmtc0: Doubleword Move To CP0 */
/*			 0x06-0x0a */
# define SO_MFMC0	 0x0b	/* (C0) WHAT? */
/*			 0x0c-0x0f */
# define SO_CO		 0x10	/* (C0) CP0 Exception Related Instructions */
#  define FN_TLBR	  0x01	/* (C0) tlbr: TLB Read */
#  define FN_TLBWI	  0x02	/* (C0) tlbwi: TLB Write Indexed */
#  define FN_TLBWR	  0x06	/* (C0) tlbwr: TLB Write Random */
#  define FN_TLBP	  0x08	/* (C0) tlbp: TLB Probe */
#  define FN_ERET	  0x18	/* (C0) eret: Exception RETurn */
#  define FN_WAIT	  0x20	/* (C0) wait: Standby */
#define OP_BEQL		0x14	/* (I)  beql: BEQ Likely */
#define OP_BNEL		0x15	/* (I)  bnel: BNE Likely */
#define OP_BLEZL	0x16	/* (I)  blezl: BLEZ Likely */
#define OP_BGTZL	0x17	/* (I)  bgtzl: BGTZ Likely */
#define OP_DADDI	0x18	/* (I)  daddi: Doubleword ADDI */
#define OP_DADDIU	0x19	/* (I)  daddiu: Doubleword ADDIU */
#define OP_LDL		0x1a	/* (I)  ldl: Load Doubleword Left */
#define OP_LDR		0x1b	/* (I)  ldr: Load Doubleword Right */
#define OP_SPECIAL2	0x1c	/* Another set of R-instructions */
# define FN_MADD	 0x00	/* (R)  madd: Multiply ADD */
# define FN_MADDU	 0x01	/* (R)  maddu: MADD Unsigned */
# define FN_MUL		 0x02	/* (R)  mul: MULtiply to GPR */
/*			 0x03 */
# define FN_MSUB	 0x04	/* (R)  msub: Multiply SUB */
# define FN_MSUBU	 0x05	/* (R)  msubu: MSUB Unsigned */
# define FN_CLZ		 0x20	/* (R)  clz: Count Leading Zeros */
# define FN_CLO		 0x21	/* (R)  clo: Count Leading Ones */
/*			 0x22 */
/*			 0x23 */
# define FN_DCLZ	 0x24	/* (R)  dclz: Doubleword CLZ */
# define FN_DCLO	 0x25	/* (R)  dclo: Doubleword CLO */
#define OP_SPECIAL3	0x1f	/* Yet another set of R-instructions */
# define FN_EXT		 0x00	/* (XI) ext: EXTract bit field */
# define FN_DEXTM	 0x01	/* (XI) dextm: Doubleword EXTract Middle */
# define FN_DEXTU	 0x02	/* (XI) dextu: Doubleword EXTract Upper */
# define FN_DEXT	 0x03	/* (XI) dext: Doubleword EXTract bit field */
# define FN_INS		 0x04	/* (XI) ins: INSert bit field */
# define FN_DINSM	 0x05	/* (XI) dinsm: Doubleword INSert Middle */
# define FN_DINSU	 0x06	/* (XI) dinsu: Doubleword INSert Upper */
# define FN_DINS	 0x07	/* (XI) dins: Doubleword INSert bit field */
# define FN_BSHFL	 0x20	/* X-instructions */
#  define SO_WSBH	  0x02	/* (X)  wsbh: Word Swap Bytes in Hwords */
#  define SO_SEB	  0x10	/* (X)  seb: Sign-Extend Byte */
#  define SO_SEH	  0x18	/* (X)  seh: Sign-Extend Halfword */
# define FN_DBSHFL	 0x24	/* Doubleword X-instructions */
#  define SO_DSBH	  0x02	/* (X)  dsbh: Dword Swap Bytes in Hwords */
#  define SO_DSHD	  0x05	/* (X)  dshd: Dword Swap Hwords in Dword */
# define FN_RDHWR	 0x3b	/* (R)  rdhwr: ReaD HardWare Register */
#define OP_LB		0x20	/* (I)  lb: Load Byte */
#define OP_LH		0x21	/* (I)  lh: Load Halfword */
#define OP_LWL		0x22	/* (I)  lwl: Load Word Left */
#define OP_LW		0x23	/* (I)  lw: Load Word */
#define OP_LBU		0x24	/* (I)  lbu: Load Byte Unsigned */
#define OP_LHU		0x25	/* (I)  lhu: Load Halfword Unsigned */
#define OP_LWR		0x26	/* (I)  lwr: Load Word Right */
#define OP_LWU		0x27	/* (I)  lwu: Load Word Unsigned */
#define OP_SB		0x28	/* (I)  sb: Store Byte */
#define OP_SH		0x29	/* (I)  sh: Store Halfword */
#define OP_SWL		0x2a	/* (I)  swl: Store Word Left */
#define OP_SW		0x2b	/* (I)  sw: Store Word */
#define OP_SDL		0x2c	/* (I)  sdl: Store Doubleword Left */
#define OP_SDR		0x2d	/* (I)  sdr: Store Doubleword Right */
#define OP_SWR		0x2e	/* (I)  swr: Store Word Right */
#define OP_CACHE	0x2f	/* (I)  cache: Cache operation */
#define OP_LL		0x30	/* (I)  ll: Load Linked */
#define OP_LLD		0x34	/* (I)  lld: Load Linked Doubleword */
#define OP_LD		0x37	/* (I)  ld: Load Doubleword */
#define OP_SC		0x38	/* (I)  sc: Store Conditional */
#define OP_SCD		0x3c	/* (I)  scd: Store Conditional Doubleword */
#define OP_SD		0x3f	/* (I)  sd: Store Doubleword */

#endif
