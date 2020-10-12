P_ONE_LOOP_BASE(8)
type_sew_t<8>::type pd = 0; \
type_sew_t<8>::type ps1 = P_FIELD(rs1, (i ^ 1), type_sew_t<8>::type); 
pd = ps1;
WRITE_PD(); 
P_LOOP_END() 