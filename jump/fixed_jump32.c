#include "jump32.h"
#include "tinymt32.h"
#include "polynomial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void fixed_jump(tinymt32_t * tiny, const char * poly_str,
		uint64_t lower_step,
		uint64_t upper_step,
		uint32_t seed);

void fixed_jump(tinymt32_t * tiny, const char * poly_str,
		uint64_t lower_step,
		uint64_t upper_step,
		uint32_t seed)
{
    tinymt32_t new_tiny_z;
    tinymt32_t * new_tiny = &new_tiny_z;
    char jump_str[100];
    polynomial charcteristic;
    polynomial jump_poly;
    polynomial tee;

    strtop(&charcteristic, poly_str);
    tee.ar[0] = 2;
    tee.ar[1] = 0;
    polynomial_power_mod(&jump_poly,
			 &tee,
			 lower_step,
			 upper_step,
			 &charcteristic);
    memset(jump_str, 0, sizeof(jump_str));
    ptostr(jump_str, &jump_poly);
    printf("lower_step: %"PRIu64" upper_step:%"PRIu64"\n",
	   lower_step, upper_step);
    printf("jump_poly:%s\n", jump_str);
    printf("check data:\n");
    printf("seed = %u\n", seed);
    tinymt32_init(tiny, seed);
    *new_tiny = *tiny;
    printf("before:\n");
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 5; j++) {
	    printf("%10"PRIu32" ", tinymt32_generate_uint32(tiny));
	}
	printf("\n");
    }
    /* period jump */
    tinymt32_jump(new_tiny,
		  lower_step,
		  upper_step,
		  poly_str);
    printf("after:\n");
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 5; j++) {
	    printf("%10"PRIu32" ", tinymt32_generate_uint32(new_tiny));
	}
	printf("\n");
    }
}

int main(int argc, char * argv[]) {
    tinymt32_t tiny;
    uint32_t seed = 1;
    uint64_t lower;
    uint64_t upper;
    if (argc < 8) {
	printf("usage:\n%s polynomial mat1 mat2 tmat lower upper seed\n",
	       argv[0]);
	return -1;
    }
    tiny.mat1 = strtoul(argv[2], NULL, 16);
    tiny.mat2 = strtoul(argv[3], NULL, 16);
    tiny.tmat = strtoul(argv[4], NULL, 16);
    lower = strtoull(argv[5], NULL, 10);
    upper = strtoull(argv[6], NULL, 10);
    seed = strtoul(argv[7], NULL, 10);
    printf("characteristic polynomial:%s\n", argv[1]);
    printf("mat1:%08"PRIx32"\n", tiny.mat1);
    printf("mat2:%08"PRIx32"\n", tiny.mat2);
    printf("tmat:%08"PRIx32"\n", tiny.tmat);
    printf("seed:%d\n", seed);
    fixed_jump(&tiny, argv[1], lower, upper, seed);
}
