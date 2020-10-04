#include "devices.h"
#include "processor.h"
#include "uart.h"

#include <unistd.h>
#include <sys/ioctl.h>

bool uart_t::load(reg_t addr, size_t len, uint8_t* bytes) {
    if (len > 1) {
        printf("[UART] Load more than 1 byte ... \n");
        return false;
    }


    if (uart_lcr & UART_LCR_DLAB) {
        switch (addr) {
            case UART_DLL : // 0  
                memcpy(bytes, &uart_dll, len);
                break;
            case UART_DLM : // 1 
                memcpy(bytes, &uart_dlm, len);
                break;
            case UART_SSDR : // 5 
                memcpy(bytes, &uart_psd, len);
                break; 
            default:
                printf("[UART] Trying read 0x%lx ... \n", addr + UART_BASE);
                return false;
        }
    }
    else {
        switch (addr) {
        case UART_RHR : // 0  
            *bytes = 0;
            int amt;
            if ((ioctl(0, FIONREAD, &amt) == 0) && (amt > 0))
                read(0, bytes, len);
            break; 
        case UART_IER : // 1  
            memcpy(&uart_ier, bytes, len);
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
                printf("[UART] Trying read 0x%lx ... \n", addr + UART_BASE);
                return false;
        }
    }

    return true;
}

bool uart_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
    if (len > 1) {
        printf("[UART] Store more than 1 byte ... \n");
        return false;
    }

    if (uart_lcr & UART_LCR_DLAB) {
        switch (addr) {
            case UART_DLL : // 0  
                memcpy(&uart_dll, bytes, len);
                break;
            case UART_DLM : // 1 
                memcpy(&uart_dlm, bytes, len);
                break;
            case UART_SSDR : // 5 
                memcpy(&uart_psd, bytes, len);
                break;
            default:
                printf("[UART] Trying write 0x%lx ... \n", addr + UART_BASE);
                return false; 
        }
    }
    else {
        switch (addr) {
            case UART_THR : // 0
                putchar(*bytes); fflush(stdout);
                break; 
            case UART_IER : // 1
                memcpy(&uart_ier, bytes, len);
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
                printf("[UART] Trying write 0x%lx ... \n", addr + UART_BASE);
                return false;
        }
    }
    return true;
}