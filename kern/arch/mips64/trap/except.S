/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/asm.h>
#include <asm/ptrace.h>
#include <asm/stackframe.h>

NESTED(except_generic, TF_SIZE, sp)
	.set	push
	.set	noat
	.set	noreorder
	/*
	 * I saved everything we need into a trap frame, then deal with the
	 * exception in C code.
	 *
	 * This is to leave as much work to C as possible.
	 *
	 * MIPS specifications require that exception handler entry should
	 * take at most 128 bytes, hence the jump.
	 */
	j	handle_exception_entry
	nop
	.set	pop
	END(except_generic)

handle_exception_entry:
	.set	push
	.set	noat
	/*
	 * Save all registers and coprocessor registers we need.  The list
	 * include but is probably not limited to:
	 * 1. GPRs, except k0 and k1 reserved for exception handler, which
	 *    is usually not saved.
	 * 2. CP0 status register, indicating current CPU working state.
	 * 3. CP0 cause register, storing the cause of exception or interrupt,
	 *    and whether the victim instruction is in a branch delay slot.
	 * 4. CP0 exception program counter (EPC) register, showing the
	 *    address of instruction which threw an exception (the victim
	 *    instruction).
	 * 5. CP0 bad virtual address (BADVADDR) register, which stores the
	 *    memory address the processor tries to access if the exception
	 *    is an access violation.
	 * 6. CP0 ENTRYLO0, ENTRYLO1, and ENTRYHI registers, in case of a
	 *    failed TLB refill (will be covered in later labs).
	 *
	 * The macro SAVE_ALL defined in asm/stackframe.h allocates a
	 * trap frame structure on kernel stack and stores everything we need
	 * there.
	 * The macro RESTORE_ALL_AND_RET restores the registers from the
	 * trap frame and return from exception handler via ERET instruction,
	 * which does the following atomically (or simultaneously in one
	 * cycle):
	 * 1. Turning off exception level bit (EXL) and error level bit (ERL),
	 *    both bits forcing the processor to run in kernel mode regardless
	 *    of the KSU bits.
	 * 2. Restoring program counter from EPC register.
	 */
	SAVE_ALL
	/*
	 * Turn Exception Level off and kernel mode on to deal with nested
	 * exceptions, as the processor won't save victim instruction
	 * address into EPC register when EXL=1.
	 */
	mfc0	k0, CP0_STATUS
	ori	k0, ST_EXCM
	xori	k0, ST_EXCM
	mtc0	k0, CP0_STATUS
	/*
	 * Here JAL transfer handling to C function handle_exception(), with
	 * its only argument stored in register a0, which is equal to sp,
	 * pointing to the trap frame structure.
	 */
	move	a0, sp
	jal	handle_exception
	RESTORE_ALL_AND_RET
	.set	pop


