P_ONE_LOOP_BASE(8)
auto pd = P_FIELD(rs1, (i ^ 1), 8); 
WRITE_PD();
P_LOOP_END()