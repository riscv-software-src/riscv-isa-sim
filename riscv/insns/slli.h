if(xpr64)
  RD = RS1 << SHAMT;
else
{
  if(SHAMT & 0x20)
    throw trap_illegal_instruction;
  RD = sext32(RS1 << SHAMT);
}
