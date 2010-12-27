if(int32_t(RS2) == 0 || (int32_t(RS1) == INT32_MIN && int32_t(RS2) == -1))
  RD = sext32(int32_t(RS1) < 0 ? INT32_MIN : INT32_MAX);
else
  RD = sext32(int32_t(RS1)/int32_t(RS2));
