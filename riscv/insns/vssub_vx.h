// vssub: Saturating subs of signed integers
VI_VX_LOOP
({
    int64_t result = rs1 - vs2;
    if (result >= (int64_t)(2^(sew - 1)))
        result = (2^(sew - 1)) - 1;
    vd = result;
})
