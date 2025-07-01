#include "vdefs.h"

//vf arr_d1[VLEN] = {4, 0x3f80, 3.9,  1.4125, 5.7};
//vf arr_d2[VLEN] = {0, 0x3f80, 6.78, 6.4532, 5.7};
vf arr_d1[VLEN] = {0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,  0xffffffffffffffff, 0xffffffffffffffff};
vf arr_d2[VLEN] = {0, 1, 2, 3, 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
vf arr_dr[VLEN] = {};

void print_res(vf arg1[], vf arg2[], vf arg3[], int len, int* temp_ptr);

int vl = VLEN; 

int temp = 0;
int mask = 0;

int* temp_ptr = &temp;

int main(){
	__asm__("li %[vl], 0x6200;"
			"csrr  %[temp], mstatus;"
			"li %[mask], 0xFFFFFFFFFFFF99FF;"
			"and %[temp], %[mask], %[temp];"
			"or %[temp], %[vl], %[temp];"
			"sw %[temp], (%[temp_ptr]);"
			"csrw  mstatus, %[temp];"
			"li %[vl], 128;"
			#if VTYPE == FLOAT_T || VTYPE == INT_T 
				"vsetvli %[vl], %[vl], e32, m1, ta, ma;"
				"vle32.v v0, (%[arr_d1]);"
				"vle32.v v2, (%[arr_d2]);"
			#elif VTYPE == DOUBLE_T
				"vsetvli %[vl], %[vl], e64, m1, ta, ma;"
				"vle64.v v0, (%[arr_d1]);"
				"vle64.v v2, (%[arr_d2]);"
			#elif VTYPE == SHORT_T
				"vsetvli %[vl], %[vl], e16, m1, ta, ma;"
				"vle16.v v1, (%[arr_d1]);"
				"vle16.v v2, (%[arr_d2]);"
			#endif
				//"vsetvli %[vl], %[vl], e32, m1, ta, ma;"
			"vnsra.wv v3, v0, v2;"

			#if VTYPE == FLOAT_T || VTYPE == INT_T 
				"vse32.v v3, (%[arr_dr]);"
			#elif VTYPE == DOUBLE_T
				"vse64.v v3, (%[arr_dr]);"
			#elif VTYPE == SHORT_T
				"vse16.v v3, (%[arr_dr]);"
			#endif
			: // out
			: [mask] "r" (mask), [temp_ptr] "r" (temp_ptr), [temp] "r" (temp), [arr_dr] "r" (&arr_dr),  [arr_d1] "r" (arr_d1) , [arr_d2] "r" (arr_d2), [vl] "r" (vl)
			: "memory"
	);
	print_res(arr_d1, arr_d2, arr_dr, vl, temp_ptr);

	return 0;
}
