/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#define UART_BASE		0xbfe80000
#define MASTER_INPUT_CLOCK	(125000000 / 16)
#define BAUD_RATE		115200

#include <config.h>
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <drivers/serial/uart16550.h>

/*
 * During initializations Some of the GPRs are reserved for improving
 * readability, and reducing code redundancy:
 *
 * v0 - always UART_BASE
 * v1 - the character to be printed to serial
 * t8 - for spinning line status register
 * t9 - holding temporary ra
 *
 * This is reasonable since writing to serial console takes that amount
 * of registers for control anyway.
 */

/*
 * void initserial(void)
 * Initialize serial
 */
LEAF(initserial)
	li	v0, UART_BASE

	/* Disable interrupts */
	sb	zero, UART_INTR_ENABLE(v0)

	/* 
	 * Set parity = none, data length = 8bit, stop bit = 1bit, and
	 * ready to set baud rate.
	 */
	li	a1, UART_LCR_DLAB | UART_LCR_PARITY_NONE | \
		UART_LCR_DATA_8BIT | UART_LCR_STOP_1BIT
	sb	a1, UART_LINE_CONTROL(v0)

	/* Set baud rate: MSB:LSB = Master Input Clock / Baud Rate */
	sb	zero, UART_DIVISOR_MSB(v0)
	li	a0, MASTER_INPUT_CLOCK / BAUD_RATE
	sb	a0, UART_DIVISOR_LSB(v0)

	/* Disable Divisor Latch Access */
	xori	a1, UART_LCR_DLAB
	sb	a1, UART_LINE_CONTROL(v0)

	/* Disable FIFO */
	sb	zero, UART_FIFO_CONTROL(v0)

	/* Ready Terminal */
	li	a1, UART_MCR_DTRC | UART_MCR_RTSC
	sb	a1, UART_MODEM_CONTROL(v0)

	jr	ra
END(initserial)

LEAF(charserial)
	li	v0, UART_BASE
1:	lb	t8, UART_LINE_STATUS(v0)
	andi	t8, UART_LSR_THRE
	beqz	t8, 1b
	sb	v1, UART_TRANS_HOLD(v0)
	jr	ra
END(charserial)

/*
 * void stringserial(const char *str)
 * Print a string to the serial console
 */
LEAF(stringserial)
	move	t9, ra
1:	lb	v1, (a0)
	beqz	v1, 2f
	bal	charserial
	addiu	a0, 1
	b	1b
2:	jr	t9
END(stringserial)

/*
 * void hexserial32(const unsigned int)
 * Print a hexadecimal to the serial
 */
LEAF(hexserial32)
	move	t9, ra
	li	a1, 32
1:	addiu	a1, -4
	srlv	a2, a0, a1
	andi	a2, 0xf
	lb	v1, __digits(a2)
	bal	charserial
	bnez	a1, 1b
	jr	t9
END(hexserial32)

LEAF(udecserial)
	bnez	a0, 1f
	lb	v1, __digits(zero)
	bal	charserial
	jr	ra

1:	move	t9, ra
	move	a1, zero
	move	a2, zero
	move	t1, zero
	move	t2, zero
	li	a3, 10
	li	t3, 32

1:	divu	a0, a3
	mflo	a0
	mfhi	v1
	beq	t1, t3, 2f
	sll	a1, 4
	addu	a1, v1
	addiu	t1, 4
	b	3f
2:	sll	a2, 4
	addu	a2, v1
	addiu	t2, 4
3:	bnez	a0, 1b

	beqz	t1, 9f
1:	addiu	t1, -4
	andi	v1, a1, 0xf
	srl	a1, 4
	lb	v1, __digits(v1)
	bal	charserial
	bnez	t1, 1b

9:	beqz	t2, 9f
1:	addiu	t2, -4
	andi	v1, a2, 0xf
	srl	a2, 4
	lb	v1, __digits(v1)
	bal	charserial
	bnez	t2, 1b

9:	jr	t9
END(udecserial)

/* Do *NOT* forget the null character */
DEFINE_STRING(__digits, "0123456789abcdef\0")
