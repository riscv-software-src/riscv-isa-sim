require_extension('P');
reg_t rs1 = (xlen == 32) ? RS1_PAIR : RS1;
reg_t lsb = get_field(RS2, make_mask64(0, 5));
WRITE_RD(sext32(get_field(rs1, make_mask64(lsb, 32))));