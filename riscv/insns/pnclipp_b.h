require_extension('P');
require_rv64;
reg_t rd_tmp = 0;
reg_t s_low = RS1;
reg_t s_high = RS2;
for (int i = 0; i < 8; i++) {
  sreg_t s_h = (i < 4) ? P_FIELD(s_low, i, 16) : P_FIELD(s_high, i - 4, 16);
  sreg_t sat_val = P_SAT(8, s_h);
  if (sat_val != s_h) P.VU.vxsat->write(1);
  rd_tmp = set_field(rd_tmp, make_mask64(i * 8, 8), (uint8_t)sat_val);
}
WRITE_RD(rd_tmp);

