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
 * void initserial(void)
 * Initialize serial
 */
LEAF(initserial)
	li	a0, UART_BASE

	/* Disable interrupts */
	sb	zero, UART_INTR_ENABLE(a0)

	/* 
	 * Set parity = none, data length = 8bit, stop bit = 1bit, and
	 * ready to set baud rate.
	 */
	li	a1, UART_LCR_DLAB | UART_LCR_PARITY_NONE | \
		UART_LCR_DATA_8BIT | UART_LCR_STOP_1BIT
	sb	a1, UART_LINE_CONTROL(a0)

	/* Set baud rate: MSB:LSB = Master Input Clock / Baud Rate */
	sb	zero, UART_DIVISOR_MSB(a0)
	li	v0, MASTER_INPUT_CLOCK / BAUD_RATE
	sb	v0, UART_DIVISOR_LSB(a0)

	/* Disable Divisor Latch Access */
	xori	a1, UART_LCR_DLAB
	sb	a1, UART_LINE_CONTROL(a0)

	/* Disable FIFO */
	sb	zero, UART_FIFO_CONTROL(a0)

	/* Ready Terminal */
	li	a1, UART_MCR_DTRC | UART_MCR_RTSC
	sb	a1, UART_MODEM_CONTROL(a0)

	jr	ra
END(initserial)

/*
 * void stringserial(const char *str)
 * Print a string to the serial console
 */
LEAF(stringserial)
	li	v0, UART_BASE
2:
	lb	a1, (a0)
	beqz	a1, 8f
	/* Wait for UART_LSR_THRE flag to be set */
1:	lb	v1, UART_LINE_STATUS(v0)
	andi	v1, UART_LSR_THRE
	beqz	v1, 1b
	/* Write to data register */
	sb	a1, UART_TRANS_HOLD(v0)
	addiu	a0, 1
	b	2b
8:
	jr	ra
END(stringserial)

/*
 * void hexserial32(const unsigned int)
 * Print a hexadecimal to the serial
 */
LEAF(hexserial32)
	li	v0, UART_BASE
	li	v1, 32
2:
	addiu	v1, -4
	srlv	a1, a0, v1
	lb	a2, __digits(a1)

1:	lb	a3, UART_LINE_STATUS(v0)
	andi	a3, UART_LSR_THRE
	beqz	a3, 1b

	sb	a2, UART_TRANS_HOLD(v0)

	bnez	v1, 2b

	jr	ra
END(hexserial32)

LEAF(decserial)
	jr	ra
END(decserial)

/* Do *NOT* forget the null character */
DEFINE_STRING(__digits, "0123456789abcdef\0")
