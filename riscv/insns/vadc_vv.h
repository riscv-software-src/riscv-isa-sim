//  vd[i] = vs1[i] + vs2[i] + v0[i].LSB
//  v0[i] = carry(vs1[i] + vs2[i] + v0[i].LSB)
VI_VV_LOOP
({
    uint64_t &v0 = p->VU.elt<uint64_t>(0, i); 
    int64_t sum = sext_xlen(vs1 + vs2);
    vd = sext_xlen(sum + (v0&1));
    if (sum > (1 << p->VU.vsew))
        v0 |= 1; 
})
