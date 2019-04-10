// vsadd: Saturating adds of signed integers
VI_VI_LOOP
({
	int64_t comp =  (simm5 < 0) ? -(1 << (sew - 1)) : (1 << (sew -1)) - 1;
	if ((simm5 < 0) == (vs2 > (comp - simm5))) {
		vd = simm5 + vs2;
	}else{
		vd = (simm5 < 0) ? -(1 << (sew - 1)) : (1 << (sew -1)) - 1; 
	}		
})
