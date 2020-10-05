#include "devices.h"
#include "processor.h"
#include "uart.h"

#include <unistd.h>
#include <sys/ioctl.h>

bool uart_t::load(reg_t addr, size_t len, uint8_t* bytes) {
    if (len > 1) {
        printf("[UART] Load 0x%016lx with %ld byte (>1) \n", addr, len);
        return false;
    }

    *bytes = 0;

    switch (addr) {
        case UART_RHR : // 0  
            if (uart_lcr & UART_LCR_DLAB) memcpy(bytes, &uart_dll, len);
            else {
                int amt;
                if ((ioctl(0, FIONREAD, &amt) == 0) && (amt > 0))
                    read(0, bytes, len);
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
            if ((ioctl(0, FIONREAD, &amt) == 0) && (amt > 0))
                *bytes |= UART_LSR_DR; 
            break; 
        case UART_MSR : // 6  
            memcpy(&uart_msr, bytes, len);
            break; 
        case UART_SPR : // 7  
            memcpy(&uart_spr, bytes, len);
            break; 
        default:
                printf("[UART] Load illegal address 0x%016lx[%x] \n", addr + UART_BASE, *bytes);
                return false;
    }


    return true;
}

bool uart_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
    if (len > 1) {
        printf("[UART] Store 0x%016lx with %ld byte (>1) \n", addr, len);
        return false;
    }

    switch (addr) {
        case UART_THR : // 0
            if (uart_lcr & UART_LCR_DLAB) memcpy(&uart_dll, bytes, len);
            else { putchar(*bytes); fflush(stdout); }
            
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
                printf("[UART] Store illegal address 0x%016lx[%x] \n", addr + UART_BASE, *bytes);
                return false;
            } 
    }

    return true;
}