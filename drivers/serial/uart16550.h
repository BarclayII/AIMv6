/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _DRIVERS_SERIAL_UART16550_H
#define _DRIVERS_SERIAL_UART16550_H

/*
 * UART 16550A driver
 *
 * References:
 * https://www.freebsd.org/doc/en_US.ISO8859-1/articles/serial-uart/
 * http://ecee.colorado.edu/~ecen2120/Manual/uart/UART.html
 */

/*
 * Include config.h to obtain UART base address (through
 * asm/mach-$MACH/config.h)
 */
#include <config.h>

#define UART16550_BAUD_2400	2400
#define UART16550_BAUD_4800	4800
#define UART16550_BAUD_9600	9600
#define UART16550_BAUD_19200	19200
#define UART16550_BAUD_38400	38400
#define UART16550_BAUD_57600	57600
#define UART16550_BAUD_115200	115200

#ifndef UART16550_REG_OFFSET
#define UART16550_REG_OFFSET	1
#endif

/*
 * TODO: verify if this register specification, as given in Loongson 2H
 * manual, is compatible to others.
 */
#define UART_RCV_BUFFER		0	/* R/O */
#define UART_TRANS_HOLD		0	/* W/O */
#define UART_INTR_ENABLE	1
#define UART_INTR_ID		2	/* R/O */
#define UART_FIFO_CONTROL	2	/* W/O */
#define UART_LINE_CONTROL	3
#define UART_MODEM_CONTROL	4	/* W/O? */
#define UART_LINE_STATUS	5	/* R/O */
#define UART_MODEM_STATUS	6	/* R/O */

#define UART_DIVISOR_LSB	0	/* DLAB = 1 */
#define UART_DIVISOR_MSB	1	/* DLAB = 1 */

/* Interrupt Enable Register contents */
#define UART_IER_DSSI		0x08	/* Modem Status Interrupt */
#define UART_IER_LSI		0x04	/* Line Status Interrupt */
#define UART_IER_THREI		0x02	/* Transmitter Holding Register Empty */
#define UART_IER_RBFI		0x01	/* Receive Buffer Available */

/* Interrupt Identifier Register contents */
#define UART_IIR_IMASK		0x0e	/* AND this to get interrupt type */
# define UART_IIR_RECVERR	0x06	/* Receive Error */
# define UART_IIR_RBF		0x04	/* Data Available */
# define UART_IIR_STALE		0x0c	/* Stale Data */
# define UART_IIR_TBE		0x02	/* Transmit Buffer has more room */
# define UART_IIR_DSS		0x00	/* Modem Status Change */
#define UART_IIR_nIP		0x01	/* Interrupt Pending (negative) */

/* FIFO Control Register contents */
#define UART_FCR_RTB_1		0x00	/* Trigger when received 1 byte */
#define UART_FCR_RTB_4		0x40	/* ... 4 bytes */
#define UART_FCR_RTB_8		0x80	/* ... 8 bytes */
#define UART_FCR_RTB_14		0xc0	/* ... 14 bytes */
#define UART_FCR_RST_TRANSMIT	0x04	/* Reset Transmit FIFO */
#define UART_FCR_RST_RECEIVER	0x02	/* Reset Receiver FIFO*/
#define UART_FCR_ENABLE		0x01	/* Enable FIFO */

/* Line Control Register Masks */

/* Divisor Latch Access Bit */
#define UART_LCR_DLAB		0x80

/* Break Control Bit */
#define UART_LCR_BREAK		0x40

/* Parity selection */
#define UART_LCR_PARITY_NONE	0x00
#define UART_LCR_PARITY_ODD	0x08
#define UART_LCR_PARITY_EVEN	0x18
#define UART_LCR_PARITY_MARK	0x28
#define UART_LCR_PARITY_SPACE	0x38

/* Data length */
#define UART_LCR_DATA_5BIT	0x0
#define UART_LCR_DATA_6BIT	0x1
#define UART_LCR_DATA_7BIT	0x2
#define UART_LCR_DATA_8BIT	0x3

/* Stop bit selection */
#define UART_LCR_STOP_1BIT	0x0
#define UART_LCR_STOP_2BIT	0x4

/* Modem Control Register contents */
#define UART_MCR_LOOPBACK_CTL	0x10	/* Loopback control */
#define UART_MCR_OUT2		0x08	/* OUT 2 signal, for interrupts */
#define UART_MCR_OUT1		0x04	/* OUT 1 signal */
#define UART_MCR_RTSC		0x02	/* Request-To-Send control */
#define UART_MCR_DTRC		0x01	/* Data-Terminal-Ready control */

/* Line Status Register contents */
#define UART_LSR_ERROR		0x80	/* Error occurred */
#define UART_LSR_TEMT		0x40	/* Transmitter Empty */
#define UART_LSR_THRE		0x20	/* Transmitter Holding Register Empty */
#define UART_LSR_BREAK_INTR	0x10	/* Break Interrupt */
#define UART_LSR_FRAME_ERROR	0x08	/* self-descriptive */
#define UART_LSR_PARITY_ERROR	0x04	/* ditto */
#define UART_LSR_OVERRUN_ERROR	0x02	/* ditto */
#define UART_LSR_DATA_READY	0x01	/* ditto */

/* Modem Status Register contents, we don't care mostly */

/* 
 * The following four bits are marked complement in Loongson 2H manual, somehow
 * contradicts the description in the references.
 */
#define UART_MSR_DCD		0x80	/* Data Carrier Detect */
#define UART_MSR_RI		0x40	/* Ring Indicator */
#define UART_MSR_DSR		0x20	/* Data Set Ready */
#define UART_MSR_CTS		0x10	/* Clear To Send */
/* Deltas */
#define UART_MSR_DDCD		0x08
#define UART_MSR_TERI		0x04	/* RI low-to-high transition */
#define UART_MSR_DDSR		0x02
#define UART_MSR_DCTS		0x01

#ifndef __ASSEMBLER__

#include <sys/types.h>
#include <asm/io.h>

/*
 * TODO: implement in8() and out8() somewhere.
 * i386 CPUs access UART via in and out instructions, while MIPS do so
 * by reading and writing memory-mapped registers.
 */
#define UART_READ(reg)		in8(UART_BASE + (reg))
#define UART_WRITE(reg, data)	out8(UART_BASE + (reg), data)

void uart_init(void);
void uart_enable(void);
void uart_disable(void);
unsigned char uart_spin_getbyte(void);
void uart_spin_putbyte(unsigned char byte);

#endif

#endif
