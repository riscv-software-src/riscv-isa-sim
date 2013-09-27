if(xpr64)
  WRITE_RD(RS1 >> SHAMT);
else
{
  if(SHAMT & 0x20)
    throw trap_illegal_instruction();
  WRITE_RD(sext32((uint32_t)RS1 >> SHAMT));
}
