P_64_UPROFILE({
  rd = rs1 + rs2;
  if (rd < rs1) {
    rd >>= 1;
    rd |= ((reg_t)1 << 63);
  } else {
    rd >>= 1;
  }
})