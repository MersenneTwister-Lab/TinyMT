#include "jump32.h"
#include "tinymt32.h"
#include <stdio.h>
#include <stdlib.h>

void test(tinymt32_t * tiny, const char * poly_str, uint32_t seed);
int check_tiny(tinymt32_t *a, tinymt32_t *b);

int check_tiny(tinymt32_t *a, tinymt32_t *b)
{
    int check = 0;
    for (int i = 0; i < 10; i++) {
	uint32_t x = tinymt32_generate_uint32(a);
	uint32_t y = tinymt32_generate_uint32(b);
	if (x != y) {
	    printf("%d[%08x,%08x]\n", i, x, y);
	    check |= 1;
	}
    }
    if (check == 0) {
	printf("OK!\n");
    } else {
	printf("NG!\n");
    }
    return check;
}

void test(tinymt32_t * tiny, const char * poly_str, uint32_t seed)
{
    tinymt32_t new_tiny_z;
    tinymt32_t * new_tiny = &new_tiny_z;
    uint64_t test_count = 100;
    uint64_t test[] = {3, 1279, 100003};

    tinymt32_init(tiny, seed);
    *new_tiny = *tiny;
    /* period jump */
    printf("jump period\n");
    tinymt32_jump(new_tiny,
		  UINT64_C(0xffffffffffffffff),
		  UINT64_C(0x7fffffffffffffff),
		  poly_str);
    if (check_tiny(new_tiny, tiny)) {
	    return;
    }
    /* plus jump */
    for (int index = 0; index < 3; index++) {
	test_count = test[index];
	printf("jump %"PRId64"[0000000000000000%016"PRIx64"]\n",
	       test_count, test_count);
	*new_tiny = *tiny;
	for (unsigned int i = 0; i < test_count; i++) {
	    tinymt32_generate_uint32(tiny);
	}
	tinymt32_jump(new_tiny, test_count, 0, poly_str);
	if (check_tiny(new_tiny, tiny)) {
	    return;
	}
    }
    /* minus jump */
    for (int index = 0; index < 3; index++) {
	test_count = UINT64_C(0xffffffffffffffff) - test[index];
	printf("jump (period - %"PRId64") [7fffffffffffffff%016"PRIx64"]\n",
	       test[index], test_count);
	*new_tiny = *tiny;
	tinymt32_jump(new_tiny,
		      test_count,
		      UINT64_C(0x7fffffffffffffff),
		      poly_str);
	for (unsigned int i = 0; i < test[index]; i++) {
	    tinymt32_generate_uint32(new_tiny);
	}
	if (check_tiny(new_tiny, tiny)) {
	    return;
	}
    }
    /* one polynomial two jump */
    f2_polynomial jump_poly;
    f2_polynomial jump_poly2;
    for (int index = 0; index < 3; index++) {
	test_count = test[index];
	printf("jump %"PRId64"[0000000000000000%016"PRIx64"]\n",
	       test_count, test_count);
	*new_tiny = *tiny;
	calculate_jump_polynomial(&jump_poly,
				  test_count, 0, poly_str);
	calculate_jump_polynomial(&jump_poly2,
				  test_count * 2, 0, poly_str);
	tinymt32_jump_by_polynomial(new_tiny, &jump_poly);
	tinymt32_jump_by_polynomial(new_tiny, &jump_poly);
	tinymt32_jump_by_polynomial(tiny, &jump_poly2);
	if (check_tiny(new_tiny, tiny)) {
	    return;
	}
    }
    /* one polynomial two jump (minus jump)*/
    uint64_t test_count2;
    for (int index = 0; index < 3; index++) {
	test_count = UINT64_C(0xffffffffffffffff) - test[index];
	test_count2 = UINT64_C(0xffffffffffffffff) - 2 * test[index];
	printf("jump (period - %"PRId64") [7fffffffffffffff%016"PRIx64"]\n",
	       test[index], test_count);
	printf("jump (period - %"PRId64") [7fffffffffffffff%016"PRIx64"]\n",
	       test[index] * 2, test_count2);
	*new_tiny = *tiny;
	calculate_jump_polynomial(&jump_poly,
				  test_count,
				  UINT64_C(0x7fffffffffffffff),
				  poly_str);
	calculate_jump_polynomial(&jump_poly2,
				  test_count2,
				  UINT64_C(0x7fffffffffffffff),
				  poly_str);
	tinymt32_jump_by_polynomial(new_tiny, &jump_poly);
	tinymt32_jump_by_polynomial(new_tiny, &jump_poly);
	tinymt32_jump_by_polynomial(tiny, &jump_poly2);
	if (check_tiny(new_tiny, tiny)) {
	    return;
	}
    }
}

int main(int argc, char * argv[]) {
    if (argc < 5) {
	printf("usage:\n%s polynomial mat1 mat2 tmat [seed]\n", argv[0]);
	return -1;
    }
    tinymt32_t tiny;
    tiny.mat1 = strtoul(argv[2], NULL, 16);
    tiny.mat2 = strtoul(argv[3], NULL, 16);
    tiny.tmat = strtoul(argv[4], NULL, 16);
    uint32_t seed = 1;
    if (argc > 5) {
	seed = strtoul(argv[5], NULL, 10);
    }
    printf("characteristic polynomial:%s\n", argv[1]);
    printf("mat1:%08"PRIx32"\n", tiny.mat1);
    printf("mat2:%08"PRIx32"\n", tiny.mat2);
    printf("tmat:%08"PRIx32"\n", tiny.tmat);
    printf("seed:%d\n", seed);
    test(&tiny, argv[1], seed);
}
