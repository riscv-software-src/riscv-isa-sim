#include "encoding.h"

#define ZERO	0
#define T0      5
#define S0      8
#define S1      9

static uint32_t bits(uint32_t value, unsigned int hi, unsigned int lo) {
  return (value >> lo) & ((1 << (hi+1-lo)) - 1);
}

static uint32_t bit(uint32_t value, unsigned int b) {
  return (value >> b) & 1;
}

static uint32_t jal(unsigned int rd, uint32_t imm) __attribute__ ((unused));
static uint32_t jal(unsigned int rd, uint32_t imm) {
  return (bit(imm, 20) << 31) |
    (bits(imm, 10, 1) << 21) |
    (bit(imm, 11) << 20) |
    (bits(imm, 19, 12) << 12) |
    (rd << 7) |
    MATCH_JAL;
}

static uint32_t csrsi(unsigned int csr, uint16_t imm) __attribute__ ((unused));
static uint32_t csrsi(unsigned int csr, uint16_t imm) {
  return (csr << 20) |
    (bits(imm, 4, 0) << 15) |
    MATCH_CSRRSI;
}

static uint32_t sw(unsigned int src, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t sw(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (src << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SW;
}

static uint32_t sd(unsigned int src, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t sd(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (src << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SD;
}

static uint32_t sh(unsigned int src, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t sh(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (src << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SH;
}

static uint32_t sb(unsigned int src, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t sb(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (src << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SB;
}

static uint32_t ld(unsigned int rd, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t ld(unsigned int rd, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LD;
}

static uint32_t lw(unsigned int rd, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t lw(unsigned int rd, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LW;
}

static uint32_t lh(unsigned int rd, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t lh(unsigned int rd, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LH;
}

static uint32_t lb(unsigned int rd, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t lb(unsigned int rd, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LB;
}

static uint32_t csrw(unsigned int source, unsigned int csr) __attribute__ ((unused));
static uint32_t csrw(unsigned int source, unsigned int csr) {
  return (csr << 20) | (source << 15) | MATCH_CSRRW;
}

static uint32_t addi(unsigned int dest, unsigned int src, uint16_t imm) __attribute__ ((unused));
static uint32_t addi(unsigned int dest, unsigned int src, uint16_t imm)
{
  return (bits(imm, 11, 0) << 20) |
    (src << 15) |
    (dest << 7) |
    MATCH_ADDI;
}

static uint32_t csrr(unsigned int rd, unsigned int csr) __attribute__ ((unused));
static uint32_t csrr(unsigned int rd, unsigned int csr) {
  return (csr << 20) | (rd << 7) | MATCH_CSRRS;
}

static uint32_t fsw(unsigned int src, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t fsw(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_FSW;
}

static uint32_t fsd(unsigned int src, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t fsd(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_FSD;
}

static uint32_t flw(unsigned int dest, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t flw(unsigned int dest, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(dest, 4, 0) << 7) |
    MATCH_FLW;
}

static uint32_t fld(unsigned int dest, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t fld(unsigned int dest, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(dest, 4, 0) << 7) |
    MATCH_FLD;
}

static uint32_t ebreak(void) __attribute__ ((unused));
static uint32_t ebreak(void) { return MATCH_EBREAK; }
static uint32_t ebreak_c(void) __attribute__ ((unused));
static uint32_t ebreak_c(void) { return MATCH_C_EBREAK; }

static uint32_t dret(void) __attribute__ ((unused));
static uint32_t dret(void) { return MATCH_DRET; }

static uint32_t fence_i(void) __attribute__ ((unused));
static uint32_t fence_i(void)
{
  return MATCH_FENCE_I;
}

/*
static uint32_t lui(unsigned int dest, uint32_t imm) __attribute__ ((unused));
static uint32_t lui(unsigned int dest, uint32_t imm)
{
  return (bits(imm, 19, 0) << 12) |
    (dest << 7) |
    MATCH_LUI;
}

static uint32_t csrci(unsigned int csr, uint16_t imm) __attribute__ ((unused));
static uint32_t csrci(unsigned int csr, uint16_t imm) {
  return (csr << 20) |
    (bits(imm, 4, 0) << 15) |
    MATCH_CSRRCI;
}

static uint32_t li(unsigned int dest, uint16_t imm) __attribute__ ((unused));
static uint32_t li(unsigned int dest, uint16_t imm)
{
	return addi(dest, 0, imm);
}

static uint32_t fsd(unsigned int src, unsigned int base, uint16_t offset) __attribute__ ((unused));
static uint32_t fsd(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_FSD;
}

static uint32_t ori(unsigned int dest, unsigned int src, uint16_t imm) __attribute__ ((unused));
static uint32_t ori(unsigned int dest, unsigned int src, uint16_t imm)
{
  return (bits(imm, 11, 0) << 20) |
    (src << 15) |
    (dest << 7) |
    MATCH_ORI;
}

static uint32_t nop(void) __attribute__ ((unused));
static uint32_t nop(void)
{
  return addi(0, 0, 0);
}
*/

static uint32_t xori(unsigned int dest, unsigned int src, uint16_t imm) __attribute__ ((unused));
static uint32_t xori(unsigned int dest, unsigned int src, uint16_t imm)
{
  return (bits(imm, 11, 0) << 20) |
    (src << 15) |
    (dest << 7) |
    MATCH_XORI;
}

static uint32_t srli(unsigned int dest, unsigned int src, uint8_t shamt) __attribute__ ((unused));
static uint32_t srli(unsigned int dest, unsigned int src, uint8_t shamt)
{
	return (bits(shamt, 4, 0) << 20) |
		(src << 15) |
		(dest << 7) |
		MATCH_SRLI;
}
