// vmul
VI_VX_LOOP
({
    WRITE_RD(sext_xlen(rs1 * vs2));
})
