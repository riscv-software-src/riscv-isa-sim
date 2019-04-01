// vsadd: Saturating adds of signed integers
VI_VI_LOOP
({
    int64_t result = simm5 + vs2;
    if (result >= (int64_t)(2^(sew - 1)))
        result = (2^(sew - 1)) - 1;
    vd = result;
})
