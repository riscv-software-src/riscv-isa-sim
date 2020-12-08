P_ONE_LOOP_BASE(16)
auto pd = P_FIELD(rs1, (i ^ 1), 16);
WRITE_PD();
P_LOOP_END()