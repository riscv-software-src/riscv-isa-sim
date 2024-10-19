#include "crc_dev.h"
#include <iostream>

crc_dev_t crc_module;

/**********************************************************************************
 * PUBLIC FUNCTION DEFINATION
 **********************************************************************************/
crc_dev_t::crc_dev_t()
{
    std::cout << "Initialising CRC Module" << std::endl;
}

#include <iostream>
bool crc_dev_t::load(reg_t addr, size_t len, uint8_t *bytes)
{
    uint32_t dummy;
    /* Make sure the buffer length of size crc_t */
    if(len != sizeof(crc_t))
        return false;
    std::cout << "Test\n";
    switch (addr)
    {
    case MMIO_CRC_RESULT:
        memcpy(bytes, &u32_crc_res, sizeof(u32_crc_res));
        break;
    case MMIO_CRC_CR:
        std::cout << "Test2\n";
        *bytes = csr_u.csr_r & 0xFFFF;
        //memcpy(bytes, &dummy, sizeof(csr_u.csr_r));
        break;
    case MMIO_CRC_SR:
        dummy = (csr_u.csr_r >> 16) & 0xFFFF;
        memcpy(bytes, &dummy, sizeof(csr_u.csr_r));
        break;
    case MMIO_CRC_SET_POLY:
        memcpy(bytes, &u32_polynomial, sizeof(u32_crc_res));
        break;
    default:
        return false;
    }
    return true;
}

bool crc_dev_t::store(reg_t addr, size_t len, const uint8_t *bytes)
{
    crc_t dummy;
    switch (addr)
    {
    case MMIO_CRC_CR:
        memcpy(&dummy, bytes, len);
        csr_u.csr_r = dummy & 0xFFFF; // Write lower 16 bits of control register
        break;
    case MMIO_CRC_SR:
        memcpy(&dummy, bytes, len);
        if(dummy & (1 << 0))
            csr_u.csr_s.s_int = false; // Reset the int status register
        break;
    case MMIO_CRC_SET_POLY:
        /* Copy polynomial value to polynomial register */
        memcpy(&u32_polynomial, bytes, len);
        break;
    case MMIO_CRC_DATA:
        /* Copy the address of the buffer pointer */
        memcpy(u8p_data, bytes, sizeof(uint8_t));
        /* Set data length */
        u32_data_length = (crc_t)len;
        return hw_crc_convert();
    default:
        return false;
    }
    return true;
}

void crc_dev_t::tick(reg_t UNUSED rtc_ticks)
{
    return;
}

/**********************************************************************************
 * PRIVATE FUNCTION DEFINATION
 **********************************************************************************/
/**
 * @brief  Perform the hw crc_conversion as per polynomial type
 */
bool crc_dev_t::hw_crc_convert()
{
    crc_t crc = UINT32_MAX;
    crc_t crc_width;

    if (!csr_u.csr_s.c_en || csr_u.csr_s.s_busy)
    {
        /* return if not enable or already busy */
        return false;
    }
    /* Raise the busy flag as we will start the conversion */
    csr_u.csr_s.s_busy = true;

    switch (csr_u.csr_s.c_poly_type)
    {
    case CRC_POLYNOMIAL_TYPE_32:
        crc = UINT32_MAX;
        crc_width = 32;
        break;
    case CRC_POLYNOMIAL_TYPE_16:
        crc = UINT16_MAX;
        crc_width = 32;
        break;
    case CRC_POLYNOMIAL_TYPE_8:
        crc = 0x00;
        crc_width = 32;
        break;
    default:
        return false;
    }
    for (size_t i = 0; i < u32_data_length; i++)
    {
        crc ^= (*((volatile uint8_t *)u8p_data + i) << (crc_width - 8)); // Align input byte with CRC size
        for (int j = 0; j < 8; j++)
        {
            if (crc & (1U << (crc_width - 1))) // Check highest bit
                crc = (crc << 1) ^ u32_polynomial;
            else
                crc <<= 1;
        }
        crc &= (1U << crc_width) - 1; // Keep CRC constrained to its size
    }
    u32_crc_res = crc;
    /* Release busy flag */
    csr_u.csr_s.s_busy = false;
    /* Raise interrupt */
    if(!csr_u.csr_s.s_int)
    {
        csr_u.csr_s.s_int = true;
        raise_intr();
    }
    else
    {
        /* Race condition */
    }
    return true;
}

void crc_dev_t::raise_intr()
{
    /* TODO */
}