require_supervisor_hwacha;
h->get_ct_state()->reset();
for (uint32_t i=0; i<h->max_uts; i++)
  h->get_ut_state(i)->reset();
