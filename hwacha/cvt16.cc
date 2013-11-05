#include "cvt16.h"

#define H_BIAS    (UINT16_C(0xf))
#define H_F_MASK  (UINT16_C(0x03FF))
#define H_E_MASK  (UINT16_C(0x7C00))
#define H_E_SHIFT (10)
#define H_S_MASK  (UINT16_C(0x8000))

#define H_QNAN    (H_F_MASK)

#define S_BIAS    (UINT32_C(0x7F))
#define S_F_MASK  (UINT32_C(0x007fffff))
#define S_E_MASK  (UINT32_C(0x7f800000))
#define S_E_SHIFT (23)
#define S_S_MASK  (UINT32_C(0x80000000))

#define S_QNAN    (S_F_MASK)

#define PAD  (S_E_SHIFT - H_E_SHIFT)

uint_fast32_t cvt_hs(uint_fast16_t x)
{
#define MSB (UINT32_C(0x00800000))
	uint_fast32_t frac, exp, sign;
	frac = (x & H_F_MASK) << PAD;
	exp  = (x & H_E_MASK);
	sign = (x & H_S_MASK);

	switch (exp) {
	case 0:
		if (frac) { /* Denormal */
			exp = S_BIAS - 14;
			/* Adjust fraction for implicit leading 1-bit */
			for (; !(frac & MSB); frac <<= 1, exp--);
			frac &= ~(MSB);
			exp <<= S_E_SHIFT;
		}
		break;

	case H_E_MASK: /* Infinity and NaN */
		exp = S_E_MASK;
		if (frac) { /* Set padding bits for NaN */
			frac |= (1 << PAD) - 1;
		}
		break;
	default:
		exp += (S_BIAS - H_BIAS) << H_E_SHIFT; /* Re-bias */
		exp <<= PAD;
	}
	return (sign << 16) | exp | frac;
#undef MSB
}

enum riscv_rm {
	RNE = 0, /* Round to nearest; ties to even */
	RTZ = 1, /* Round towards zero (truncate) */
	RDN = 2, /* Round towards negative infinity (down) */
	RUP = 3, /* Round towards positive infinity (up) */
	RMM = 4, /* Round to nearest; ties to max magnitude */
};

/*
 * LSB           : frac[13]
 * Guard bit  (G): frac[12]
 * Round bit  (R): frac[11]
 * Sticky bit (S): OR of frac[10..0]
 *
 * RTZ:
 *   truncate
 * RUP:
 *   000 : exact
 *  else : round up
 * RDN:
 *   000 : exact
 *  else : round down
 * RNE:
 *   0xx : round down 
 *   100 : tie; round up if LSB is 1
 *   101 : round up
 *   110 : round up
 *   111 : round up
 */
uint_fast16_t cvt_sh(uint_fast32_t x, int rm)
{
#define MSB UINT16_C(0x0400)
	uint_fast32_t frac, exp, sign;
	int e;
	sign = (x & S_S_MASK) >> 16;
	exp = (x & S_E_MASK);
	if (exp && exp != S_E_MASK) {
		int inc;
		inc = 0;
		switch (rm) {
		case RNE:
			/* Round up if G is set and either R, S,
			   or the bit before G is non-zero */
			inc = (x & 0x1000) && (x & 0x2fff);
			break;
		case RUP:
			inc = ((x & 0x1fff) != 0) && (!sign);
			break;
		case RDN:
			inc = ((x & 0x1fff) != 0) && sign;
			break;
		}
		x += inc << PAD;
		exp = (x & S_E_MASK);
	}
	frac = (x & S_F_MASK) >> PAD;

	e = (exp >> S_E_SHIFT) - S_BIAS;
	if (e < -24) { /* Round to zero */
		return sign;
	} else if (e < -14) { /* Denormal */
		frac = (frac | MSB) >> (-e - 14);
		return sign | frac;
	} else if (e < 16) {
		exp = (e + H_BIAS) << H_E_SHIFT;
	} else if (e < 127) { /* Round to infinity */
		exp = H_E_MASK;
		frac = 0;
	} else {
		/* Infinity and NaN */
	}
	return sign | exp | frac;
#undef MSB
}

