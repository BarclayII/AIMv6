/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>

#ifdef UART_ZYNQ7000

#include <asm/io.h>

#include <drivers/serial/uart-zynq7000.h>

void uart_init(void)
{
	/* Disable interrupts */
	out32(UART_BASE + UART_OFFSET_IDR, UART_IXR_MASK);
	/* Disable TX and RX */
	uart_disable();
	/* Reset TX and RX, Clear FIFO */
	out32(UART_BASE + UART_OFFSET_CR, UART_CR_TXRST | UART_CR_RXRST);
	/* Clear Flags */
	out32(UART_BASE + UART_OFFSET_ISR, UART_IXR_MASK);
	/* Mode Reset to Normal/8-N-1 */
	out32(UART_BASE + UART_OFFSET_MR, 
		UART_MR_CHMODE_NORM | UART_MR_CHARLEN_8_BIT |
		UART_MR_PARITY_NONE | UART_MR_STOPMODE_1_BIT);
	/* Trigger Reset */
	out32(UART_BASE + UART_OFFSET_RXWM, UART_RXWM_RESET_VAL);
	out32(UART_BASE + UART_OFFSET_TXWM, UART_TXWM_RESET_VAL);
	/* Disable RX timeout */
	out32(UART_BASE + UART_OFFSET_RXTOUT, UART_RXTOUT_DISABLE);
	/* Reset baud rate generator and divider to genetate 115200 */
	out32(UART_BASE + UART_OFFSET_BAUDGEN, 0x3E);
	out32(UART_BASE + UART_OFFSET_BAUDDIV, 0x06);
	/* Set CR Value */
	out32(UART_BASE + UART_OFFSET_CR, 
		UART_CR_RX_DIS | UART_CR_TX_DIS | UART_CR_STOPBRK);
}

void uart_enable(void)
{
	/* Enable TX and RX */
	out32(UART_BASE + UART_OFFSET_CR, UART_CR_TX_EN | UART_CR_RX_EN);
}

void uart_disable(void)
{
	/* Disable TX and RX */
	out32(UART_BASE + UART_OFFSET_CR, UART_CR_TX_DIS | UART_CR_RX_DIS);
}

unsigned char uart_spin_getbyte(void)
{
	while (in32(UART_BASE + UART_OFFSET_SR) & UART_SR_RXEMPTY);
	return in32(UART_BASE + UART_OFFSET_FIFO);
}

void uart_spin_putbyte(unsigned char byte)
{
	while (in32(UART_BASE + UART_OFFSET_SR) & UART_SR_TXFULL);
	out32(UART_BASE + UART_OFFSET_FIFO, byte);
}

#endif	/* UART_ZYNQ7000 */
