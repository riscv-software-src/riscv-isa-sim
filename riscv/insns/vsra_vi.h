// vsra.vi vd, vs2, zimm5
VI_VI_LOOP
({
  vd = vs2 >> (insn.v_zimm5() & (sew - 1));
})
