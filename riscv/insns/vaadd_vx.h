// vaadd: Averaging adds of integers 
VRM xrm = p->VU.get_vround_mode();
VI_VX_LOOP
({
    int64_t tval = vsext(rs1, sew) + vsext(vs2, sew); 
	INT_ROUNDING(tval, xrm, 1);
    uint64_t result = vzext(tval >> 1, sew);

    vd = result; 
})
