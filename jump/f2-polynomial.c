/**
 * @file f2-polynomial.c
 *
 * @brief calculation of polynomial over F<sub>2</sub> of degree less than 128.
 *
 * Polynomial calculation for TinyMT jump function.
 * polynomial_power_mod is used in jump32.c and jump64.c
 * In this file, polynomial of degree less than 256 is defined,
 * but the 256-bit polynomial is used only for intermediate value,
 * multiplication between 256-bit polynomials are not supported.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2011,2012 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */
#include "f2-polynomial.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>

/**
 * This structure represents polynomial over F<sub>2</sub> of
 * degree less than 256.
 */
struct POLYNOMIAL256_T {
    uint64_t ar[4];
};

typedef struct POLYNOMIAL256_T lpol;
typedef f2_polynomial pol;

inline static void shiftup_lpol1(lpol *dest);
inline static void shiftup_lpol_n0(lpol *dest, int n);
inline static void shiftup_lpol_n1(lpol *dest, int n);
inline static void shiftup_lpol_n2(lpol *dest, int n);
inline static void shiftup_lpol_n3(lpol *dest, int n);
inline static int deg_lpol_lazy(const lpol * x, const int pre_deg);

static void mod_lpol(lpol *dest, const lpol *x);
#if defined(DEBUG)
static void print_lpol(char * title, lpol *x);
#endif

/**
 * addition of F<sub>2</sub>-polynomial<br>
 * dest = dest + src
 * @param dest destination polynomial
 * @param src source polynomial
 */
inline static void add_lpol(lpol * dest, const lpol * src) {
    dest->ar[0] ^= src->ar[0];
    dest->ar[1] ^= src->ar[1];
    dest->ar[2] ^= src->ar[2];
    dest->ar[3] ^= src->ar[3];
}

/**
 * clear polynomial.
 * dest = 0
 * @param dest polynomial set to be zero.
 */
inline static void clear_lpol(lpol * dest) {
    dest->ar[0] = 0;
    dest->ar[1] = 0;
    dest->ar[2] = 0;
    dest->ar[3] = 0;
}

/**
 * convert 128-bit polynomial to 256-bit polynomial
 * @param dest 256-bit polynomial
 * @param src 128-bit polynomial
 */
inline static void tolpol(lpol * dest, const pol * src) {
    dest->ar[0] = src->ar[0];
    dest->ar[1] = src->ar[1];
    dest->ar[2] = 0;
    dest->ar[3] = 0;
}

/**
 * shift up 1 bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t</b>
 * @param dest 256-bit polynomial
 */
inline static void shiftup_lpol1(lpol *dest) {
    uint64_t msb0 = dest->ar[0] >> 63;
    uint64_t msb1 = dest->ar[1] >> 63;
    uint64_t msb2 = dest->ar[2] >> 63;
    dest->ar[0] = dest->ar[0] << 1;
    dest->ar[1] = (dest->ar[1] << 1) | msb0;
    dest->ar[2] = (dest->ar[2] << 1) | msb1;
    dest->ar[3] = (dest->ar[3] << 1) | msb2;
}

/**
 * shift down 1 bit, if　indeterminate of dest is <b>t</b>
 * dest = dest / <b>t</b>
 * @param dest 256-bit polynomial
 */
inline static void shiftdown_lpol1(lpol *dest) {
    uint64_t lsb0 = dest->ar[1] << 63;
    uint64_t lsb1 = dest->ar[2] << 63;
    uint64_t lsb2 = dest->ar[3] << 63;
    dest->ar[0] = (dest->ar[0] >> 1) | lsb0;
    dest->ar[1] = (dest->ar[1] >> 1) | lsb1;
    dest->ar[2] = (dest->ar[2] >> 1) | lsb2;
    dest->ar[3] = (dest->ar[3] >> 1);
}

/**
 * shift up n bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t</b><sup>n</sup>
 * @param dest 256-bit polynomial
 * @param n number of shift up
 */
inline static void shiftup_lpoln(lpol * dest, int n) {
    if (n <= 64) {
	shiftup_lpol_n0(dest, n);
    } else if (n <= 128) {
	shiftup_lpol_n1(dest, n);
    } else if (n <= 192) {
	shiftup_lpol_n2(dest, n);
    } else {
	shiftup_lpol_n3(dest, n);
    }
}

/**
 * subcontract function: n < =64
 * shift up n bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t</b><sup>n</sup>
 * @param dest 256-bit polynomial
 * @param n number of shift up
 */
inline static void shiftup_lpol_n0(lpol *dest, int n) {
    uint64_t msb0 = dest->ar[0] >> (64 - n);
    uint64_t msb1 = dest->ar[1] >> (64 - n);
    uint64_t msb2 = dest->ar[2] >> (64 - n);
    dest->ar[3] = (dest->ar[3] << n) | msb2;
    dest->ar[2] = (dest->ar[2] << n) | msb1;
    dest->ar[1] = (dest->ar[1] << n) | msb0;
    dest->ar[0] = dest->ar[0] << n;
}

/**
 * subcontract function: 64 < n <= 128
 * shift up n bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t</b><sup>n</sup>
 * @param dest 256-bit polynomial
 * @param n number of shift up
 */
inline static void shiftup_lpol_n1(lpol *dest, int n) {
    n -= 64;
    uint64_t msb0 = dest->ar[0] >> (64 - n);
    uint64_t msb1 = dest->ar[1] >> (64 - n);
    dest->ar[3] = (dest->ar[2] << n) | msb1;
    dest->ar[2] = (dest->ar[1] << n) | msb0;
    dest->ar[1] = (dest->ar[0] << n);
    dest->ar[0] = 0;
}

/**
 * subcontract function: 128 < n <= 192
 * shift up n bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t</b><sup>n</sup>
 * @param dest 256-bit polynomial
 * @param n number of shift up
 */
inline static void shiftup_lpol_n2(lpol *dest, int n) {
    n -= 128;
    uint64_t msb0 = dest->ar[0] >> (64 - n);
    dest->ar[3] = (dest->ar[1] << n) | msb0;
    dest->ar[2] = (dest->ar[0] << n);
    dest->ar[1] = 0;
    dest->ar[0] = 0;
}

/**
 * subcontract function: 192 < n <= 256
 * shift up n bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t</b><sup>n</sup>
 * @param dest 256-bit polynomial
 * @param n number of shift up
 */
inline static void shiftup_lpol_n3(lpol *dest, int n) {
    n -= 192;
    dest->ar[3] = dest->ar[0] << n;
    dest->ar[2] = 0;
    dest->ar[1] = 0;
    dest->ar[0] = 0;
}

/**
 * get degree
 * if polynomial is zero return -1, else return degree of the polynomial.
 * @param x polynomial
 * @return degree
 */
inline static int deg_lpol(const lpol * x) {
    return deg_lpol_lazy(x, 255);
}

/**
 * sub function
 * pre_deg >= deg (must be)
 * @param x polynomial
 * @param pre_deg search start degree
 * @return degree
 */
inline static int deg_lpol_lazy(const lpol * x, const int pre_deg) {
    int deg = pre_deg;
    uint64_t mask;
    int index = pre_deg / 64;
    int bit_pos = pre_deg % 64;

    mask = UINT64_C(1) << bit_pos;
    for (int i = index; i >= 0; i--) {
	while (mask != 0) {
	    if ((x->ar[i] & mask) != 0) {
		return deg;
	    }
	    mask = mask >> 1;
	    deg--;
	}
	mask = UINT64_C(1) << 63;
    }
    return -1; // should be minus infinity
}

#if defined(DEBUG)
static void print_lpol(char * title, lpol *x)
{
    printf("%s MSB ", title);
    for(int i = 3; i >= 0; i--) {
	printf("%016" PRIx64, x->ar[i]);
    }
    printf(" LSB\n");
}
#endif

/**
 * multiplication of polynomials
 * y's degree is assumed to be lower than 128 <br>
 * x = x * y
 * @param x polynomial
 * @param y polynomial
 */
static void mul_pol(lpol *x, const lpol *y) {
    lpol result_z;
    lpol *result = &result_z;
    clear_lpol(result);
    uint64_t y64 = y->ar[0];
    for (int i = 0; i < 64; i++) {
	if ((y64 & 1) != 0) {
	    add_lpol(result, x);
	}
	shiftup_lpol1(x);
	y64 = y64 >> 1;
	if ((y64 == 0) && (y->ar[1] == 0)) {
	    break;
	}
    }
    y64 = y->ar[1];
    while (y64 != 0) {
	if ((y64 & 1) != 0) {
	    add_lpol(result, x);
	}
	shiftup_lpol1(x);
	y64 = y64 >> 1;
    }
    *x = *result;
}

/**
 * square polynomial <br>
 * x = x * x
 * @param x polynomial
 */
static void square_lpol(lpol *x) {
    lpol tmp;
    tmp = *x;
    mul_pol(x, &tmp);
}

/**
 * remainder of polynomial
 * dest = dest % x
 * @param dest polynomial to be divided and remainder
 * @param x divisor
 */
static void mod_lpol(lpol *dest, const lpol *x) {
    lpol tmp_z;
    lpol * tmp = &tmp_z;
    int deg = deg_lpol(x);
    int dest_deg = deg_lpol(dest);
    int diff = dest_deg - deg;
    int tmp_deg = deg;
    if (diff < 0) {
	return;
    }
    *tmp = *x;
    if (diff == 0) {
	add_lpol(dest, tmp);
	return;
    }
    shiftup_lpoln(tmp, diff);
    tmp_deg += diff;
    add_lpol(dest, tmp);
    dest_deg = deg_lpol_lazy(dest, dest_deg);
    while (dest_deg >= deg) {
	shiftdown_lpol1(tmp);
	tmp_deg--;
	if (dest_deg == tmp_deg) {
	    add_lpol(dest, tmp);
	    dest_deg = deg_lpol_lazy(dest, dest_deg);
	}
    }
}

/**
 * conversion form 256-bit polynomial to 128-bit polynomial.
 * @param dest 128-bit polynomial
 * @param x 256-bit polynomial
 */
inline static void topol(pol * dest, const lpol * x) {
    dest->ar[0] = x->ar[0];
    dest->ar[1] = x->ar[1];
}

/**
 * conversion from string to polynomial.
 * string must be in the format of TinyMTDC outputs.
 * @param poly 128-bit polynomial
 * @param str hexadecimal string of 128-bit polynomial.
 */
void strtopolynomial(f2_polynomial * poly, const char * str)
{
    char buffer[50];
    strncpy(buffer, str, 49);
    buffer[49] = 0;
    errno = 0;
    poly->ar[0] = strtoull(buffer + 16, NULL, 16);
    if (errno) {
	/* throw exception */
	perror(__FILE__);
    }
    buffer[16] = 0;
    poly->ar[1] = strtoull(buffer, NULL, 16);
    if (errno) {
	/* throw exception */
	perror(__FILE__);
    }
}

/**
 * conversion from polynomial to string.
 * @param str hexadecimal string of 128-bit polynomial.
 * @param poly 128-bit polynomial
 */
void polynomialtostr(char * str, const f2_polynomial * poly)
{
    sprintf(str, "%016"PRIx64"%016"PRIx64"\n",
	    poly->ar[1],
	    poly->ar[0]);
}

/**
 * dest = x<sup>power</sup> % mod
 * @param dest the result of calculation
 * @param x polynomial
 * @param lower_power lower 128 bit of power
 * @param upper_power upper 128 bit of power
 * @param mod divisor polynomial
 */
void polynomial_power_mod(f2_polynomial * dest,
			  const f2_polynomial * x,
			  uint64_t lower_power,
			  uint64_t upper_power,
			  const f2_polynomial * mod)
{
    lpol tmp_z;
    lpol * tmp = &tmp_z;
    lpol result_z;
    lpol * result = & result_z;
    lpol lmod_z;
    lpol * lmod = &lmod_z;
    tolpol(tmp, x);
    tolpol(lmod, mod);
    clear_lpol(result);
    result_z.ar[0] = 1;
    for (int i = 0; i < 64; i++) {
	if ((lower_power & 1) != 0) {
	    mul_pol(result, tmp);
	    mod_lpol(result, lmod);
	}
	square_lpol(tmp);
	mod_lpol(tmp, lmod);
	lower_power = lower_power >> 1;
	if ((lower_power == 0) && (upper_power == 0)) {
	    break;
	}
    }
    while (upper_power != 0) {
	if ((upper_power & 1) != 0) {
	    mul_pol(result, tmp);
	    mod_lpol(result, lmod);
	}
	square_lpol(tmp);
	mod_lpol(tmp, lmod);
	upper_power = upper_power >> 1;
    }
    topol(dest, result);
    return;
}

/**
 * calculate jump polynomial.
 * The jump polynomial is used in tinymt32_jump_by_polynomial
 * or tinymt64_jump_by_polynomial.
 * This function is time consuming.
 * The jump polynomial calculated by this function should be used for
 * the same tinymt, in other words, for the tinymts which have the
 * same characteristic polynomial.
 * @param jump_poly the result of this calculation.
 * @param lower_step lower bit of 128-bit integer
 * @param upper_step upper bit of 128-bit integer
 * @param poly_str string of the characteristic polynomial generated by
 * tinymt32dc
 */
void calculate_jump_polynomial(f2_polynomial *jump_poly,
			       uint64_t lower_step,
			       uint64_t upper_step,
			       const char * poly_str)
{
    f2_polynomial charcteristic;
    f2_polynomial tee;

    strtopolynomial(&charcteristic, poly_str);
    tee.ar[0] = 2;
    tee.ar[1] = 0;
    polynomial_power_mod(jump_poly,
			 &tee,
			 lower_step,
			 upper_step,
			 &charcteristic);
}
