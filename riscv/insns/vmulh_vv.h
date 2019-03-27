// vmulh
VI_VV_LOOP
({
    reg_t sew = insn.v_sew();
    WRITE_RD(sext_xlen((sext_xlen(vs1) * sext_xlen(vs2)) >> (sew/2)));
})
