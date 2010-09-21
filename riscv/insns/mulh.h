require64;
int64_t rb = RS1;
int64_t ra = RS2;
RDR = (int128_t(rb) * int128_t(ra)) >> 64;
