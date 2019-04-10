// vsaddu: Saturating adds of unsigned integers
VI_VI_ULOOP
({
    if ( simm5 > (((1 << sew) - 1) - vs2)){
        vd = (1 << sew) - 1;
	}else{
    	vd = simm5 + vs2;
	}

})
