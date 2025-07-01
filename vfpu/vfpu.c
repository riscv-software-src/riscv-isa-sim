//#include "MY_softfloat_types.h"

#include <stdio.h>
#include <inttypes.h>
#include "vdefs.h"

vf arr_d1[256] = {1, 0x40, 3.9,  1.4125, 5.7};
vf arr_d2[256] = {1, 0x40, 6.78, 6.4532, 5.7};
vf arr_dr[256*256] = {};

long int arr_d1_f[256] = {};

void print_res(vf arg1[], vf arg2[], vf arg3[], int len, int* temp_ptr, int operation);

int vl = VLEN; 

int temp = 0;
int mask = 0;

int* temp_ptr = &temp;

int main()
{

	uint64_t ms, vl, vt;
	__asm__ volatile ("csrr %0, mstatus" : "=r"(ms));
	__asm__ volatile ("csrr %0, vl"      : "=r"(vl));
	__asm__ volatile ("csrr %0, vtype"   : "=r"(vt));
	printf("mstatus=0x%lx  vl=%lu  vtype=0x%lx\n", ms, vl, vt);

	for (int i=0;i<=255;i++) {
		arr_d2[i].i = (long int) i;
		arr_d1_f[i] = 0xFFFFFFFFFFFFFF00 + i;
	}

	int operation_code;
	float d1_fp = 0.0;
	#pragma clang loop unroll(disable)
	for (int operation=0; operation<=3; operation++) {

		char newline = '\n';
	    printf("OP\tsrc1\tsrc2\tres\tFLAGS%c", newline);
		for (int i=0;i<=255;i++) {
	
			for (int k=0;k<=255;k++) {
				arr_d1[k].i = (long int) i;
			}

			#pragma clang loop unroll(disable)
			for (int j=0;j<=3;j++) {

				__asm__("li %[vl], 0x6200;"
						"csrr  %[temp], mstatus;"
						"li %[mask], 0xFFFFFFFFFFFF99FF;"
						"and %[temp], %[mask], %[temp];"
						"or %[temp], %[vl], %[temp];"
						"sw %[temp], (%[temp_ptr]);"
						"csrw  mstatus, %[temp];"
						"li %[vl], 512;"
						#if VTYPE == FLOAT_T
							"vsetvli %[vl], %[vl], e32, m1, ta, ma;"
							"vle32.v v1, (%[arr_d1]);"
							"vle32.v v2, (%[arr_d2]);"
						#elif VTYPE == DOUBLE_T
							"vsetvli %[vl], %[vl], e64, m1, ta, ma;"
							"vle64.v v1, (%[arr_d1]);"
							"vle64.v v2, (%[arr_d2]);"
						#elif VTYPE == SHORT_T
							"vsetvli %[vl], %[vl], e16, m1, ta, ma;"
							"vle16.v v1, (%[arr_d1]);"
							"vle16.v v2, (%[arr_d2]);"
						#elif VTYPE == CHAR_T || VTYPE == MINI1_T || VTYPE == MINI2_T
							"vsetvli %[vl], %[vl], e8, m1, ta, ma;"
							"vle8.v v1, (%[arr_d1]);"
							"vle8.v v2, (%[arr_d2]);"
							"fld %[d1_fp], (%[arr_d1_f]);"
							""
							//"vlbu.v v1 (%[arr_d1]);"
							//"vlbu.v v2 (%[arr_d1]);"
						#endif

						"li %[operation_code], 0;"
						"beq %[operation_code], %[operation], VFADD_VV;"
						"li %[operation_code], 1;"
						"beq %[operation_code], %[operation], VFSUB_VV;"
						"li %[operation_code], 2;"
						"beq %[operation_code], %[operation], VFMUL_VV;"
						"li %[operation_code], 3;"
						"beq %[operation_code], %[operation], VFDIV_VV;"
						"li %[operation_code], 4;"
						"beq %[operation_code], %[operation], VFADD_VF;"
						"li %[operation_code], 5;"
						"beq %[operation_code], %[operation], VFSUB_VF;"
						"li %[operation_code], 6;"
						"beq %[operation_code], %[operation], VFMUL_VF;"
						"li %[operation_code], 7;"
						"beq %[operation_code], %[operation], VFDIV_VF;"

						"VFADD_VV:;"
						"vfadd.vv v3, v1, v2;"
						"j STORE;"
						"VFSUB_VV:;"
						"vfsub.vv v3, v1, v2;"
						"j STORE;"
						"VFMUL_VV:;"
						"vfmul.vv v3, v1, v2;"
						"j STORE;"
						"VFDIV_VV:;"
						"vfdiv.vv v3, v1, v2;"
						"j STORE;"

						"VFADD_VF:;"
						"vfadd.vf v3, v2, %[d1_fp];"
						"VFSUB_VF:;"
						//"vfsub.vf v3, v2, %[d1_fp];"
						"VFMUL_VF:;"
						//"vfmul.vf v3, v2, %[d1_fp];"
						"VFDIV_VF:;"
						//"vfdiv.vf v3, v2, %[d1_fp];"

                        "STORE:"
						#if VTYPE == FLOAT_T
							"vse32.v v3, (%[arr_dr]);"
						#elif VTYPE == DOUBLE_T
							"vse64.v v3, (%[arr_dr]);"
						#elif VTYPE == SHORT_T
							"vse16.v v3, (%[arr_dr]);"
						#elif VTYPE == CHAR_T || VTYPE == MINI1_T || VTYPE == MINI2_T
							"vse8.v v3, (%[arr_dr]);"
						#endif
						: // out
						: [mask] "r" (mask), [temp_ptr] "r" (temp_ptr), [temp] "r" (temp),
						  [arr_dr] "r" (&arr_dr),  [arr_d1] "r" (arr_d1) , [arr_d2] "r" (arr_d2+64*j),
						  [vl] "r" (vl), [operation_code] "r" (operation_code), [operation] "r" (operation),
						  [d1_fp] "f" (d1_fp), [arr_d1_f] "r" (&arr_d1_f[i])
						: "memory"
				);
				print_res(arr_d1, arr_d2+64*j, arr_dr, 64, temp_ptr, operation);

			}
		}
	}

	return 0;
}

void print_res(vf arg1[], vf arg2[], vf arg3[], int len, int* temp_ptr, int operation) {
	char *op_name;
	if (operation == 0) {
		op_name = "vfadd: ";
	}
	else if (operation == 1) {
		op_name = "vfsub: ";
	}
	else if (operation == 2) {
		op_name = "vfmul: ";
	}
	else if (operation == 3) {
		op_name = "vfdiv: ";
	}
	for(int i=0; i<len; i++) {
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wformat"
		printf("%s %lx\t%lx\t%2lx\n", op_name, arg1[i].i, arg2[i].i, arg3[i].i);
		#pragma clang diagnostic pop
		//for (i=51; i>052; i--)
		//	arg1_params.mnt += 1/pow(2,i);
		//printf("exponent_arg1 = %d\n", arg1_params.exp);
		//printf("mantissa_arg1 = %lx\n", arg1_params.mnt);
	}
}