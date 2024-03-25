// See LICENSE for license details.

#ifndef _RISCV_ZICFISS_H
#define _RISCV_ZICFISS_H

#define xSSE() \
  ((STATE.prv != PRV_M) && get_field(STATE.menvcfg->read(), MENVCFG_SSE) && \
  p->extension_enabled('S') && \
  ((STATE.v && get_field(STATE.henvcfg->read(), HENVCFG_SSE)) || !STATE.v) && \
  (((STATE.prv == PRV_U) && get_field(STATE.senvcfg->read(), SENVCFG_SSE)) || (STATE.prv != PRV_U)))

#define PUSH_VALUE_TO_SS(value) ({ \
    reg_t push_value = (value); \
    reg_t push_ssp_addr = STATE.ssp->read() - xlen / 8; \
    if (xlen == 32) \
      MMU.ss_store<uint32_t>(push_ssp_addr, push_value); \
    else \
      MMU.ss_store<uint64_t>(push_ssp_addr, push_value); \
    STATE.ssp->write(push_ssp_addr); \
  })

#define POP_VALUE_FROM_SS_AND_CHECK(value) \
  reg_t shadow_return_addr; \
  if (xlen == 32) \
    shadow_return_addr = MMU.ss_load<uint32_t>(STATE.ssp->read()); \
  else \
    shadow_return_addr = MMU.ss_load<uint64_t>(STATE.ssp->read()); \
  software_check(value == shadow_return_addr, SHADOW_STACK_FAULT); \
  STATE.ssp->write(STATE.ssp->read() + xlen / 8);

#endif
