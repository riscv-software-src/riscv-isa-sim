#include "crc_dev.h"
#include <iostream>

#define PTR_ACCESS(_x)  *(uint32_t*)_x

crc_dev_t crc_module;

/**********************************************************************************
 * PUBLIC FUNCTION DEFINATION
 **********************************************************************************/
crc_dev_t::crc_dev_t()
{
    std::cout << "Initialising CRC Module" << std::endl;
}

bool crc_dev_t::load(reg_t addr, size_t len, uint8_t *bytes)
{
    uint32_t dummy;
    /* Make sure the buffer length of size crc_t */
    if(len != sizeof(crc_t))
        return false;
    switch (addr)
    {
    case MMIO_CRC_RESULT:
        PTR_ACCESS(bytes) = u32_crc_res;
        break;
    case MMIO_CRC_CR:
        PTR_ACCESS(bytes) = csr_u.csr_r & 0xFFFF;
        break;
    case MMIO_CRC_SR:
        PTR_ACCESS(bytes) = (csr_u.csr_r >> 16) & 0xFFFF;
        break;
    case MMIO_CRC_SET_POLY:
        PTR_ACCESS(bytes) = u32_polynomial;
    case MMIO_CRC_SET_DATA_LEN:
    case MMIO_CRC_DATA:
        /* Read as Zero (RAZ) */
        PTR_ACCESS(bytes) = 0;
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
        dummy = PTR_ACCESS(bytes);
        csr_u.csr_r = dummy & 0xFFFF; // Write lower 16 bits of control register
        break;
    case MMIO_CRC_SET_POLY:
        /* Copy polynomial value to polynomial register */
        u32_polynomial = PTR_ACCESS(bytes);
        break;
    case MMIO_CRC_DATA:
        /* Copy the address of the buffer pointer */
        u8p_data = (uint8_t*)bytes;
    case MMIO_CRC_SET_DATA_LEN:
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
        crc_width = 16;
        break;
    case CRC_POLYNOMIAL_TYPE_8:
        crc = 0x00;
        crc_width = 8;
        break;
    default:
        return false;
    }
    for (size_t i = 0; i < u32_data_length; i++)
    {
        crc ^= (u8p_data[i] << (crc_width - 8)); // Align input byte with CRC size
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