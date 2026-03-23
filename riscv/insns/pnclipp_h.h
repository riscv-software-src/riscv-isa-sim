require_extension('P');
require_rv64;
reg_t rd_tmp = 0;
reg_t s_low = RS1;
reg_t s_high = RS2;
for (int i = 0; i < 4; i++) {
  sreg_t s_w = (i < 2) ? P_FIELD(s_low, i, 32) : P_FIELD(s_high, i - 2, 32);
  sreg_t sat_val = P_SAT(16, s_w);
  if (sat_val != s_w) P.VU.vxsat->write(1);
  rd_tmp = set_field(rd_tmp, make_mask64(i * 16, 16), (uint16_t)sat_val);
}
WRITE_RD(rd_tmp);

