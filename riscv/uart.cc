#include "devices.h"
#include "processor.h"
#include <unistd.h>
#include <sys/ioctl.h>

uart_t::uart_t(plic_t* plic, bool diffTest, std::string file_path) : diffTest(diffTest), plic(plic) {
    file_fifo.open(file_path);
    if (file_fifo.is_open()) {
        printf("[SimUART] open uart file fifo %s\n", file_path.c_str());
    }
}

unsigned int sim_uart_irq;

void uart_t::check_int() {
    int amt;
    if (file_fifo.is_open() && !file_fifo.eof()) {
        plic->plic_irq(PLIC_UART_IRQ, true);
        sim_uart_irq = true;
    }
    else if (!diffTest && ((ioctl(0, FIONREAD, &amt) == 0) && (amt > 0)) ) {
        plic->plic_irq(PLIC_UART_IRQ, true);
        sim_uart_irq = false;
    }
    else {
        plic->plic_irq(PLIC_UART_IRQ, false);
        sim_uart_irq = false;
    }
}

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


bool uart_t::load(reg_t addr, size_t len, uint8_t* bytes) {
    if (len > 1) {
        printf("[SimUART] Load 0x%016lx with %ld byte (>1) \n", addr, len);
        return false;
    }

    *bytes = 0;

    switch (addr) {
        case UART_RHR : // 0  
            if (uart_lcr & UART_LCR_DLAB) memcpy(bytes, &uart_dll, len);
            else {
                if (file_fifo.is_open()) {
                    if (!file_fifo.eof())
                        file_fifo.get(*(char*)bytes);
                    if (file_fifo.eof()) {
                        file_fifo.close();
                        fprintf(stderr, "[SimUART] close fifo \n");
                    }
                        
                }
                else if (!diffTest) {
                    int amt;
                    if ((ioctl(0, FIONREAD, &amt) == 0) && (amt > 0))
                        read(0, bytes, len);
                }
            }
            break; 
        case UART_IER : // 1  
            if (uart_lcr & UART_LCR_DLAB) memcpy(bytes, &uart_dlm, len);
            else memcpy(&uart_ier, bytes, len);
            break; 
        case UART_ISR : // 2  
            memcpy(&uart_isr, bytes, len);
            break; 
        case UART_LCR : // 3  
            memcpy(&uart_lcr, bytes, len);
            break; 
        case UART_MCR : // 4  
            memcpy(&uart_mcr, bytes, len);
            break; 
        case UART_LSR : // 5 
            int amt;
            *bytes = UART_LSR_TE | UART_LSR_THRE;
            if (file_fifo.is_open()) {
                *bytes |= !file_fifo.eof(); 
            }
            else if (!diffTest) {
                if ((ioctl(0, FIONREAD, &amt) == 0) && (amt > 0))
                    *bytes |= UART_LSR_DR; 
            }

            break; 
        case UART_MSR : // 6  
            memcpy(&uart_msr, bytes, len);
            break; 
        case UART_SPR : // 7  
            memcpy(&uart_spr, bytes, len);
            break; 
        default:
                printf("[SimUART] Load illegal address 0x%016lx[%x] \n", addr + UART_BASE, *bytes);
                return false;
    }
    return true;
}

bool uart_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
    if (len > 1) {
        printf("[SimUART] Store 0x%016lx with %ld byte (>1) \n", addr, len);
        return false;
    }

    switch (addr) {
        case UART_THR : // 0
            if (uart_lcr & UART_LCR_DLAB) memcpy(&uart_dll, bytes, len);
            else { 
                fprintf(stdout, "\x1b[34m%c\x1b[0m", *bytes);
                fflush(stdout); 
            }
            break; 
        case UART_IER : // 1
            if (uart_lcr & UART_LCR_DLAB) memcpy(&uart_dlm, bytes, len);
            else memcpy(&uart_ier, bytes, len);
            break; 
        case UART_FCR : // 2  
            memcpy(&uart_fcr, bytes, len);
            break; 
        case UART_LCR : // 3  
            memcpy(&uart_lcr, bytes, len);
            break; 
        case UART_MCR : // 4  
            memcpy(&uart_mcr, bytes, len);
            break; 
        case UART_SPR : // 7  
            memcpy(&uart_spr, bytes, len);
            break;
        default:
            if ((addr == UART_PSD) && (uart_lcr & UART_LCR_DLAB)) memcpy(&uart_psd, bytes, len);
            else {
                printf("[SimUART] Store illegal address 0x%016lx[%x] \n", addr + UART_BASE, *bytes);
                return false;
            } 
    }

    return true;
}