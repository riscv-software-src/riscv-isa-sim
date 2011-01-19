if(xpr64)
  RD = sreg_t(RS1) >> SHAMT;
else
{
  if(SHAMT & 0x20)
    throw trap_illegal_instruction;
  RD = sext32(sreg_t(RS1) >> SHAMT);
}
