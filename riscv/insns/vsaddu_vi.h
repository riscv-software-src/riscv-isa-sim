// vsaddu: Saturating adds of unsigned integers
VI_VI_LOOP
({
    uint64_t result = simm5 + vs2;
    if (result >= (uint64_t)(2^(sew - 1)))
        result = (2^(sew - 1)) - 1;
    vd = result;
})
