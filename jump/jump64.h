#ifndef JUMP64_H
#define JUMP64_H

#include "f2-polynomial.h"
#include "tinymt64.h"

void tinymt64_jump(tinymt64_t *tiny,
		   uint64_t lower_step,
		   uint64_t upper_step,
		   const char * poly_str);
void tinymt64_jump_by_polynomial(tinymt64_t *tiny,
				 f2_polynomial * jump_poly);
#endif
