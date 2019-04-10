// vsaddu: Saturating adds of unsigned integers
VI_VX_ULOOP
({
	if (rs1 > (((1 << sew) - 1) - vs2)){
        vd = (1 << sew) - 1;
	}else{
    	vd = rs1 + vs2;
	}

})
