// Copyright 2019 OCULUS VR. All rights reserved.

#include <stdio.h>

//Stream of small matrix inversion <3,3>
//Single precision floating point
#define ROW1     3
#define COL1     3
#define ITR     10 // could be tens of thousands matrix inversion


//<3,3> matrices; stored row-major in memory
float MatA[ITR * ROW1 * COL1] = {
		 0.512908935785717,   0.095045735172483,   0.115968232562751,   0.460483750542851,    0.43367104392204,  0.0780846821507833,   0.350395373697577,   0.709235197729075,    0.36925290821551,
		0.0336283780790998,   0.144922819987369,   0.431870413213908,   0.192150375429611,   0.717835527713334,   0.446034886150637,   0.471359845470339,   0.661714278003999,   0.508331533758124,
		 0.528087872792076,   0.336477257468029,   0.393336369839188,   0.572878016425706,   0.173266265156337,   0.804367887230761,   0.360822066832717,  0.0861184825590432,  0.0110806874051135,
		 0.233113217741769,   0.785946970240188,    0.63437034498638,   0.933850585917873,   0.410728826844473,   0.862388186092669,   0.226797521685621,   0.119392775054738,   0.158243659491764,
		 0.601185078081295,   0.835124674642051,   0.272025227673364,   0.117607312331725,   0.940406708950366,    0.92800150622055,   0.626098359295213,   0.415557328805121,    0.92131504878463,
		 0.542024923275236,   0.320381857603966,   0.628951346794586,   0.812885486899356,   0.657880749859792,   0.785285576612356,   0.166378317478289, 0.00176289351739124,   0.294686123865976,
		 0.789683815459574,   0.802395558358354,   0.345059346102671,   0.215886429052747,   0.862149328860524,   0.412429947598106,   0.403213089519745,   0.143770158823473,   0.710129281370961,
		 0.142832001737706,  0.0464253584139167,   0.219393119783789,    0.57745320563086,   0.270998862698208,   0.340164206149133,   0.256027037862701,   0.677885368782042,   0.139812748478638,
		 0.832863680474862,   0.921583997980498,   0.598686569649114,   0.939877741010803,    0.06225405822613,   0.125176092668053,   0.525193168560599,   0.303956606566886,   0.834589471963509,
		 0.945255290691394,     0.4440267409403,   0.600730145629835,   0.905670650259438,   0.757434983624814,    0.47155760064328,   0.606618910379065,   0.209769782242258,     0.4685940116032};


//<3,3> matrices; inversion results written back to memory row-major
float MatC[ITR * ROW1 * COL1];


/* Vanilla C implementation
   Target Kernel for optimization
   We are interested in :
    - gate count of the target core
    - Cycles per matrix inversion
    - Memory BW analysis
    - Data reuse
    - MAC array utilization
    - Division vectorization/Implementation efficiency ? IEEE-754 : Non-IEEE-754
    - Rough power-estimation per matrix inversion
*/
int main(int argc, char **argv)
{

  int z, y;
  float * a_ref = MatA;
  float * c_ref = MatC;
  float ia00,ia01,ia02, ia10, ia11, ia12, ia20, ia21, ia22;
  float deta;


  for (z = 0; z < ITR; z++) {
	  ia00 = a_ref[4]*a_ref[8]-a_ref[5]*a_ref[7];
	  ia01 = a_ref[7]*a_ref[2]-a_ref[8]*a_ref[1];
	  ia02 = a_ref[5]*a_ref[1]-a_ref[2]*a_ref[4];
	  ia10 = a_ref[6]*a_ref[5]-a_ref[3]*a_ref[8];
	  ia11 = a_ref[8]*a_ref[0]-a_ref[2]*a_ref[6];
	  ia12 = a_ref[3]*a_ref[2]-a_ref[0]*a_ref[5];
	  ia20 = a_ref[3]*a_ref[7]-a_ref[4]*a_ref[6];
	  ia21 = a_ref[6]*a_ref[1]-a_ref[7]*a_ref[0];
	  ia22 = a_ref[0]*a_ref[4]-a_ref[1]*a_ref[3];

	  deta = ia00*a_ref[0]+ia10*a_ref[1]+ia20*a_ref[2];

	  c_ref[0] = ia00/deta;
	  c_ref[1] = ia01/deta;
	  c_ref[2] = ia02/deta;
	  c_ref[3] = ia10/deta;
	  c_ref[4] = ia11/deta;
	  c_ref[5] = ia12/deta;
	  c_ref[6] = ia20/deta;
	  c_ref[7] = ia21/deta;
	  c_ref[8] = ia22/deta;


	  a_ref += (ROW1*COL1);
	  c_ref += (ROW1*COL1);
  }


  c_ref = MatC;
  for (z = 0; z < ITR; z++) {
	  printf("\n");
	  for (y = 0; y < ROW1*COL1; y++) {
		  printf("%8.15f,\t",c_ref[y]);
	  }

	  c_ref += (ROW1*COL1);
  }

  return(0);
}

