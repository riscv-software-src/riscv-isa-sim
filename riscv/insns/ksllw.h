require_extension('P');
sreg_t rs1 = sext32(RS1);
sreg_t sa = get_field(RS2, make_mask64(0, 5));
sreg_t res = rs1 << sa;

P_SAT(res, 32);
WRITE_RD(sext32(res));