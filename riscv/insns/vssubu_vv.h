// vssubu: Saturating subs of unsigned integers
VI_VV_LOOP
({
    uint64_t result = vs1 - vs2;
    if (result >= (uint64_t)(2^(sew - 1)))
        result = (2^(sew - 1)) - 1;
    vd = result;
})
