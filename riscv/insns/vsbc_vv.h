// vsbc
// # vd[i] = vs1[i] - vs2[i] - v0[i].LSB
// # v0[i] = carry(vs1[i] - vs2[i] - v0[i].LSB)
VI_VV_LOOP
({
    throw trap_unimplemented_instruction(0);
})
