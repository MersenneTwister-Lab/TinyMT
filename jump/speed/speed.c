#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdlib.h>
#include "f2-polynomial.h"
#include "tinymt32.h"
#include "jump32.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
	printf("%s lower_step upper_step\n", argv[0]);
	return -1;
    }
    tinymt32_t tiny;
    char * poly = "d8524022ed8dff4a8dcc50c798faba43";
    // set parameters
    tiny.mat1 = 0x8f7011ee;
    tiny.mat2 = 0xfc78ff1f;
    tiny.tmat = 0x3793fdff;
    // initialize
    tinymt32_init(&tiny, 1);
    int i;
    clock_t start;
    clock_t finish;
    uint64_t lower_step;
    uint64_t upper_step;
    f2_polynomial jump_poly;
    double elapsed;
    int repeat = 10000;

    lower_step = strtoull(argv[1], NULL, 10);
    upper_step = strtoull(argv[2], NULL, 10);
    printf("jump step is %"PRIu64"x 2^{64} + %"PRIu64"steps\n",
	   upper_step, lower_step);
    start = clock();
    for (i = 0; i < repeat; i++) {
	tinymt32_calculate_jump_polynomial(&jump_poly,
					   lower_step,
					   upper_step,
					   poly);
    }
    finish = clock();
    elapsed = finish - start;
    elapsed = 1000 * elapsed / CLOCKS_PER_SEC;
    elapsed = elapsed / repeat;
    printf("calculate polynomial time = %f ms\n", elapsed);
    start = clock();
    for (i = 0; i < repeat; i++) {
	tinymt32_jump_by_polynomial(&tiny, &jump_poly);
    }
    finish = clock();
    elapsed = finish - start;
    elapsed = 1000 * elapsed / CLOCKS_PER_SEC;
    elapsed = elapsed / repeat;
    printf("jump time = %f ms\n", elapsed);
    return 0;
}
