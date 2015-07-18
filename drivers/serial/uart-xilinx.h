/* TODO: add license here */

#ifndef _DRIVERS_SERIAL_UART_XILINX_H
#define _DRIVERS_SERIAL_UART_XILINX_H

/* FILLME */

void uart_init(void);
void uart_enable(void);
void uart_disable(void);
unsigned char uart_spin_getbyte(void);
void uart_spin_putbyte(unsigned char byte);

#endif
