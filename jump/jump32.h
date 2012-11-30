#ifndef JUMP32_H
#define JUMP32_H

#include "f2-polynomial.h"
#include "tinymt32.h"

void tinymt32_jump(tinymt32_t *tiny,
		   uint64_t lower_step,
		   uint64_t upper_step,
		   const char * poly_str);
void tinymt32_jump_by_polynomial(tinymt32_t *tiny,
				 f2_polynomial * jump_poly);

#endif
