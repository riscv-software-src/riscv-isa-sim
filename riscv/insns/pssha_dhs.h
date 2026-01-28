require_rv32;
sreg_t sshamt = P_FIELD(RS2, 0, 16);
P_RD_RS1_DW_LOOP(16, 16, {
  uint64_t bits_SMIN = (uint64_t{1} << (16 - 1));
  uint64_t bits_SMAX = ((uint64_t{1} << (16 - 1)) - 1);
  bool ov = false;
  int8_t m = P_FIELD(RS2, 0, 8);
  int8_t  rev = static_cast<int8_t>(m);
  rev = (m < 0) ? static_cast<uint8_t>(~m + 1u) : m;
  uint64_t mask = ((uint64_t{1} << 16) - 1);
  p_rs1 &= mask;
  if(m < 0){
    unsigned sh = (rev > 255u) ? 255u : rev;
    uint64_t sign = (p_rs1 >> (16 - 1)) & 1u;

    if(sh >= 16) p_rd = (uint16_t)(sign ? mask : 0u);
    else{
      uint64_t shifted = (p_rs1 >> sh);
      uint64_t fill = (~uint64_t{0}) << (16 - sh);
      shifted |= fill;
      p_rd = (uint16_t)(shifted & mask);
    }
  }
  else{
    if(rev==0) p_rd = (uint16_t)p_rs1;
    else if(rev >= 16){
      if(p_rs1==0) p_rd = 0;
      else{
        ov = true;
        uint64_t sign = (p_rs1 >> (16 - 1)) & 1u;
        p_rd = (uint16_t)(sign ? bits_SMIN : bits_SMAX);
      }
    }
    else{
      uint64_t sign = (p_rs1 >> (16 - 1)) & 1u;
      uint64_t top  = (p_rs1 >> (16 - rev));
      uint64_t need = sign ? ((uint64_t{1} << rev) - 1) : 0u;
      ov = (top != need);
      if(ov)
        p_rd = (uint16_t)(sign ? bits_SMIN : bits_SMAX);
      else
        p_rd =  (uint16_t)((p_rs1 << rev) & mask);
    }
  }
})