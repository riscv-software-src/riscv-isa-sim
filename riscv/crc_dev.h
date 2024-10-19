#ifndef _CRC_DEV_H_
#define _CRC_DEV_H_

#include <riscv/abstract_device.h>
#include <riscv/dts.h>
#include <riscv/sim.h>
#include <fesvr/term.h>
#include <fdt/libfdt.h>

#define CRC_DEV_MMIO_BASE 0x41000000

#define MMIO_CRC_CR CRC_DEV_MMIO_BASE + 0       // W Access to Control Register
#define MMIO_CRC_SR CRC_DEV_MMIO_BASE + 1       // R/W Access Status register; only intr flag is writable to clear it post serving of interrupt
#define MMIO_CRC_DATA CRC_DEV_MMIO_BASE + 2     // W Access Data pointer Register
#define MMIO_CRC_RESULT CRC_DEV_MMIO_BASE + 3   // R Access Result Register
#define MMIO_CRC_SET_POLY CRC_DEV_MMIO_BASE + 4 // R/W Save u32_polynomial value

typedef uint32_t crc_t;

typedef enum
{
    CRC_POLYNOMIAL_TYPE_32 = 0,
    CRC_POLYNOMIAL_TYPE_16,
    CRC_POLYNOMIAL_TYPE_8,
} crc_poly_t;

typedef struct crc_dev_csr
{
    crc_t c_en : 1;
    crc_t c_poly_type : 2; // represented by crc_poly_t
    crc_t c_res : 13;      // res for config bits
    crc_t s_int : 1;
    crc_t s_busy : 1;
} crc_dev_csr_t;

class crc_dev_t : public abstract_device_t
{
private:
    crc_t u32_polynomial;
    crc_t *u8p_data;
    crc_t u32_data_length;
    crc_t u32_crc_res;
    union crc_csr_u
    {
        uint32_t csr_r;
        crc_dev_csr_t csr_s;
    } csr_u;

    reg_t interrupt_id;
    abstract_interrupt_controller_t *intctrl;
    bool hw_crc_convert();
    void raise_intr();

public:
    crc_dev_t();
    crc_dev_t(abstract_interrupt_controller_t *intctrl, reg_t int_id) : u8p_data(0), interrupt_id(int_id), intctrl(intctrl) {}
    bool load(reg_t addr, size_t len, uint8_t *bytes) override;
    bool store(reg_t addr, size_t len, const uint8_t *bytes) override;
    void tick(reg_t UNUSED rtc_ticks) override;
};

extern crc_dev_t crc_module;

#endif /* _CRC_DEV_H_ */