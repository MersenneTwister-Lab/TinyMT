#ifndef F2_POLYNOMIAL_H
#define F2_POLYNOMIAL_H
/**
 * @file polynomial.h
 *
 * @brief calculation of polynomial over F<sub>2</sub> of degree less than 128.
 *
 * Polynomial calculation for TinyMT jump function.
 * polynomial_power_mod is used in jump32.c and jump64.c
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2011, 2012 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */

#include <stdint.h>
#include <inttypes.h>

/**
 * Polynomial over F<sub>2</sub>, whose degree is equal to or less than 128.
 * LSB of ar[0], i.e. ar[0] & 1, represent constant
 */
struct F2_POLYNOMIAL128_T {
    uint64_t ar[2];
};

typedef struct F2_POLYNOMIAL128_T f2_polynomial;

void strtopolynomial(f2_polynomial * poly, const char * str);
void polynomialtostr(char * str, const f2_polynomial * poly);
void polynomial_power_mod(f2_polynomial * dest,
			  const f2_polynomial * x,
			  uint64_t lower_power,
			  uint64_t upper_power,
			  const f2_polynomial * mod);
void calculate_jump_polynomial(f2_polynomial *jump_poly,
			       uint64_t lower_step,
			       uint64_t upper_step,
			       const char * poly_str);
#endif
