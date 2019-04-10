// vsaddu: Saturating adds of unsigned integers
VI_VV_ULOOP
({
    if (vs1 > (((1 << sew) - 1) - vs2)){
        vd = (1 << sew) - 1;
	}else{
    	vd = vs1 + vs2;
	}
})
