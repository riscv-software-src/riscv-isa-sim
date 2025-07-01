#include <stdint.h>

typedef struct { uint8_t v; } float8_1_t;    //1-4-3

typedef struct { uint8_t v; } float8_2_t;    //1-5-2

#ifndef VLEN
#define VLEN 256  // number of elements
#endif

#define CHAR_T 'C'
#define SHORT_T 'S'
#define INT_T 'I'
#define FLOAT_T 'F'
#define DOUBLE_T 'D'
#define MINI1_T 'P'
#define MINI2_T 'Q'

#ifndef VTYPE
#define VTYPE 'C' // type of elements
#endif

#if VTYPE == 'D'
	#define TYPE double
	#define TYPE2 long int
#elif VTYPE == 'F'
	#define TYPE float
	#define TYPE2 int
#elif VTYPE == 'I'
	#define TYPE int
	#define TYPE2 int
#elif VTYPE == 'S'
	#define TYPE short
	#define TYPE2 short
#elif VTYPE == 'C'
	#define TYPE char
	#define TYPE2 char
#elif VTYPE == MINI1_T
	#define TYPE float8_1_t
	#define TYPE2 char
#elif VTYPE == MINI2_T
	#define TYPE float8_2_t
	#define TYPE2 char
#endif

typedef TYPE vtype;
typedef TYPE2 vtype2;

typedef union{
	vtype f;
	vtype2 i;
} vf;
