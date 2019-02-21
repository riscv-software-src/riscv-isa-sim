// Copyright 2019 OCULUS VR. All rights reserved.

#include <stdio.h>

//Stream of small matrix multiplication <2,3> x <3,3>
//Single precision floating point
#define ROW1     2
#define COL1     3
#define COL2     3
#define ITR     10 // could be tens of thousands matrix multiplication


//<2,3> matrices; stored row-major in memory
float MatA[ITR * ROW1 * COL1] = {
		 51.291,   35.04,  43.367,  46.048,  9.5046,  70.924,
		 11.597,  36.925,  19.215,  7.8085,  3.3628,  47.136,
		 14.492,  66.171,  44.603,  71.784,  43.187,  50.833,
		 52.809,  36.082,  17.327,  57.288,  33.648,  8.6118,
		 39.334,  1.1081,  93.385,  80.437,  23.311,   22.68,
		 78.595,  11.939,  86.239,  41.073,  63.437,  15.824,
		 60.119,   62.61,  94.041,  11.761,  83.512,  41.556,
		 27.203,  92.132,  81.289,    92.8,  54.202,  16.638,
		 32.038, 0.17629,  78.529,  65.788,  62.895,  29.469,
		 78.968,  40.321,  86.215,  21.589,   80.24,  14.377};

//<3,3> matrices; stored row-major in memory
float MatB[ITR * COL1 * COL2] ={
		 34.506,  14.283,  4.6425,  41.243,  57.745,    27.1,  71.013,  25.603,  67.789,
		 21.939,  83.286,  92.158,  34.016,  93.988,  6.2254,  13.981,  52.519,  30.396,
		 59.869,  94.526,  44.403,  12.518,  90.567,  75.743,  83.459,  60.662,  20.977,
		 60.073,  65.955,  90.535,  47.156,  10.752,  21.974,  46.859,   13.53,  24.752,
		 12.538,  30.153,  74.998,  24.127,  75.334,   88.44,  97.675,  43.266,  19.242,
		 96.968,  70.479,  10.717,  46.172,  39.308,  20.708,  21.085,  52.276,  37.158,
		 12.316,  88.328,   14.27,  90.805,  23.244,  40.885,  52.436,  68.103,  58.185,
		  12.33,  3.7413,  27.107,  36.459,   79.45,  82.003,  73.684,  19.428,  31.804,
		 35.846,  96.769,  86.254,  70.547,  2.8305,  64.642,  75.242,   72.29,  35.787,
		 67.285,  88.284,  19.994,  59.334,  93.403,  35.618,  26.078,  30.054,   28.95};

//<2,3> matrices; multiplication results written back to memory row-major
float MatC[ITR * ROW1 * COL2];


/* Vanilla C implementation
   Target Kernel for optimization
   We are interested in :
    - gate count of the target core
    - Cycles per matrix multiplication
    - Memory BW analysis
    - MAC array utilization
    - Rough power-estimation per matrix multiplication
*/
int main(int argc, char **argv)
{

  int z, y, x, k;
  float * a_ref = MatA;
  float * b_ref = MatB;
  float * c_ref = MatC;

  for (z = 0; z < ITR; z++) {
	  for (y = 0; y < ROW1; y++) {
	      for (x = 0; x < COL2; x++) {
  		      c_ref[COL2 * y + x] = 0;
		      for (k = 0; k < COL1; k++) {
				  c_ref[COL2 * y + x] += a_ref[COL1 * y + k] * b_ref[COL2 * k + x];
		      }
	      }
	  }

	  a_ref += (ROW1*COL1);
	  b_ref += (COL1*COL2);
	  c_ref += (ROW1*COL2);
  }


  c_ref = MatC;
  for (z = 0; z < ITR; z++) {
	  printf("\n");
	  for (y = 0; y < ROW1*COL2; y++) {
		  printf("%8.3f,\t",c_ref[y]);
	  }

	  c_ref += (ROW1*COL2);
  }

  return(0);
}

