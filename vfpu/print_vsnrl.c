#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "vdefs.h"

const int bias = 1023;

typedef struct{
	int  exp: 11;
	unsigned long int  mnt: 52;
	int  sign: 1;
} dfloat;

//int pow(int a, int b) {
//	for (int i=0; i<b; i++) {
//		a *= a;
//	}
//	return a;
//}


void print_res(vf arg1[], vf arg2[], vf arg3[], int len, int* temp_ptr) {
	//printf("temp_ptr = 0x%x\n", *temp_ptr);
	for(int i=0; i<len; i++) {
		printf("vsnrl: vs1=%lx, vs2=%lx  vd=%x\n", arg1[i].i, arg2[i].i, arg3[i].i);
		//for (i=51; i>052; i--)
		//	arg1_params.mnt += 1/pow(2,i);
		//printf("exponent_arg1 = %d\n", arg1_params.exp);
		//printf("mantissa_arg1 = %lx\n", arg1_params.mnt);

	}
}



//void print_res(const vtype arg1[], const vtype arg2[], vtype arg3[], int len) {
//	for(int i=0; i<len; i++) {
//		printf("vadd: %d + %d = %d\n", arg1[i], arg2[i], arg3[i]);
//	}
//}
