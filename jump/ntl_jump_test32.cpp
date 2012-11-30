#include <stdint.h>
#include <inttypes.h>
#include <ntl/GF2X.h>
#include <ntl/ZZ.h>
#include <ntl/vec_GF2.h>
#include <stdio.h>
#include <stdlib.h>
extern "C" {
#include "tinymt32.h"
}

using namespace NTL;
using namespace std;

static void tinymt32_add(tinymt32_t *dist, const tinymt32_t *src)
{
    for (int i = 0; i < 4; i++) {
	dist->status[i] ^= src->status[i];
    }
}

void minpoly(GF2X & poly, tinymt32_t * tiny) {
    tinymt32_init(tiny, 3317);
    vec_GF2 vec;
    int mexp = 127;
    poly.SetMaxLength(128);
    vec.SetLength(mexp * 2);
    for (int i = 0; i < mexp * 2; i++) {
        vec[i] = tinymt32_generate_uint32(tiny) & 1;
    }
    MinPolySeq(poly, vec, mexp);
}

void read_pols(GF2X & poly, char str[])
{
    uint64_t p1;
    uint64_t p2;
    p1 = strtoull(str + 16, NULL, 16);
    str[16] = 0;
    p2 = strtoull(str, NULL, 16);
    printf("p1: %016"PRIx64"\n", p1);
    printf("p2: %016"PRIx64"\n", p2);
    for (int i = 0; i < 64; i++) {
	if ((p1 & 1) == 0) {
	    SetCoeff(poly, i, 0);
	} else {
	    SetCoeff(poly, i, 1);
	}
	p1 = p1 >> 1;
    }
    for (int i = 0; i < 64; i++) {
	if ((p2 & 1) == 0) {
	    SetCoeff(poly, i + 64, 0);
	} else {
	    SetCoeff(poly, i + 64, 1);
	}
	p2 = p2 >> 1;
    }
}

int check_tiny(tinymt32_t * a, tinymt32_t * b)
{
    int check = 0;
    for (int i = 0; i < 10; i++) {
	uint32_t x = tinymt32_generate_uint32(a);
	uint32_t y = tinymt32_generate_uint32(b);
	printf("%d[%08x,%08x]\n", i, x, y);
	if (x != y) {
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

void tinymt32_jump(tinymt32_t & tiny,
		   ZZ & jump_count,
		   GF2X & poly) {
    GF2X x(1, 1);
    GF2X y;
    PowerMod(y, x, jump_count, poly);
    tinymt32_t result;
    result = tiny;
    result.status[0] = 0;
    result.status[1] = 0;
    result.status[2] = 0;
    result.status[3] = 0;
    long degree = deg(y);
    for (int i = 0; i <= degree; i++) {
	if (IsOne(coeff(y, i))) {
	    tinymt32_add(&result, &tiny);
	}
	tinymt32_next_state(&tiny);
    }
    tiny = result;

}

void test(tinymt32_t * tiny, GF2X & poly, uint32_t seed)
{
    tinymt32_t new_tiny_z;
    tinymt32_t * new_tiny = &new_tiny_z;
    const uint64_t test_count = 126;

    tinymt32_init(tiny, seed);
    *new_tiny = *tiny;
    ZZ jump_count;
    jump_count = 1;
    jump_count <<= 127;
    jump_count--;
    tinymt32_jump(*new_tiny, jump_count, poly);
    if (check_tiny(new_tiny, tiny)) {
	printf("NG but continue\n");
	//return;
    }
    jump_count = test_count;
    *new_tiny = *tiny;
    for (unsigned int i = 0; i < test_count; i++) {
	tinymt32_generate_uint32(tiny);
    }
    tinymt32_jump(*new_tiny, jump_count, poly);
    if (check_tiny(new_tiny, tiny)) {
	return;
    }
}

int main(int argc, char * argv[]) {
    if (argc < 5) {
	printf("usage:\n%s polynomial mat1 mat2 tmat [seed]\n", argv[0]);
	return -1;
    }
    GF2X poly1;
    GF2X poly2;
    tinymt32_t tiny;
    tiny.mat1 = strtoul(argv[2], NULL, 16);
    tiny.mat2 = strtoul(argv[3], NULL, 16);
    tiny.tmat = strtoul(argv[4], NULL, 16);
    uint32_t seed = 1;
    read_pols(poly1, argv[1]);
    minpoly(poly2, &tiny);
    if (poly1 != poly2) {
	cout << "poly1:\n" << poly1 << endl;
	cout << "poly2:\n" << poly2 << endl;
    }
    test(&tiny, poly2, seed);
}
