// See LICENSE for license details.

#ifndef _UART_H
#define _UART_H

/* ns16550a Register offsets */
#define UART_RHR   0x00  // Receiver Holding Register 
#define UART_THR   0x00  // Transmitter Holding Register 
#define UART_IER   0x01  // Interrupt Enable Register 
#define UART_ISR   0x02  // Interrupt Status Register
#define UART_FCR   0x02  // FIFO Control Register 
#define UART_LCR   0x03  // Line Control Register 
#define UART_MCR   0x04  // Modem Control Register 
#define UART_LSR   0x05  // Line Status Register 
#define UART_MSR   0x06  // Modem Status Register 
#define UART_SPR   0x07  // Scratch Pad Register 
#define UART_DLL   0x00  // Divisor LSB (LCR_DLAB) 
#define UART_DLM   0x01  // Divisor MSB (LCR_DLAB) 
#define UART_PSD   0x05  // Prescaler's Division Factor (LCR_DLAB) 


/* LCR Register */
#define UART_LCR_STOP   0x40  // Divisor Latch Bit
#define UART_LCR_5BIT   0x00  // 5-bit 
#define UART_LCR_6BIT   0x01  // 6-bit 
#define UART_LCR_7BIT   0x02  // 7-bit 
#define UART_LCR_8BIT   0x03  // 8-bit 
#define UART_LCR_PNONE  0x00  // No Parity
#define UART_LCR_PODD   0x08  // Parity Odd 
#define UART_LCR_PEVEN  0x18  // Parity Odd 
#define UART_LCR_PF_1   0x28  // Parity Force 1 
#define UART_LCR_PF_0   0x38  // Parity Force 0 
#define UART_LCR_DLAB   0x80  // Divisor Latch Bit


/* LSR Register */
#define UART_LSR_DR     0x01  // Data Ready 
#define UART_LSR_OE     0x02  // Overrun Error 
#define UART_LSR_PE     0x04  // Parity Error 
#define UART_LSR_FE     0x08  // Framing Error
#define UART_LSR_BI     0x10  // Break Interrupt 
#define UART_LSR_THRE   0x20  // THR Empty 
#define UART_LSR_TE     0x40  // Transmitter Empty 
#define UART_LSR_FIFOE  0x80  // FIFO Data Error

#endif /* _UART_H */