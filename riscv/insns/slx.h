require_extension('P');
if(xlen == 64){
    int shamt = RS2 & 63;
    if(shamt == 0){
        WRITE_RD((uint64_t)RD);
    }else{
        WRITE_RD(((uint64_t)RS1 >> (64 - shamt)) | ((uint64_t)RD << shamt));
    }
}else{
    int shamt = (uint32_t)RS2 & 31;
    if (shamt == 0) {
        WRITE_RD((uint32_t)RD);
    } else {
        WRITE_RD(((uint32_t)RS1 >> (32 - shamt)) | ((uint32_t)RD << shamt));
    }
}