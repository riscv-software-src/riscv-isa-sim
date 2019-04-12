// vsra.vi vd, vs2, zimm5
VI_VI_LOOP
({
  V_LOOP_ELEMENT_SKIP;

  vd = vs2 >> simm5;
})
