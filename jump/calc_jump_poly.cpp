#include <stdint.h>
#include <inttypes.h>
#include <ntl/GF2X.h>
#include <ntl/ZZ.h>
#include <ntl/vec_GF2.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

using namespace NTL;
using namespace std;

static void
read_pols(GF2X & poly, char str[]);

static void
poly2array(uint32_t array[4], GF2X& jump);

static void
pritarray(uint32_t array[4]);

static void
read_pols(GF2X & poly, char str[])
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

static void
poly2array(uint32_t array[4], GF2X& jump)
{
    if (deg(jump) > 127) {
	cerr << "error deg(jump) > 127 deg:" << dec << deg(jump) << endl;
	return;
    }
    for (int i = 0; i < 4; i++) {
	array[i] = 0;
	uint32_t mask = 1;
	for (int j = 0; j < 32; j++) {
	    if (coeff(jump, i * 32 + j) == 1) {
		array[i] |= mask;
	    }
	    mask <<= 1;
	}
    }
}

static void
pritarray(uint32_t array[4])
{
    cout << "{";
    for (int i = 0; i < 4; i++) {
	cout << "0x" << hex << setfill('0') << setw(8) << array[i];
	if (i < 3) {
	    cout << ",";
	}
    }
    cout << "}," << endl;
}

int main(int argc, char * argv[]) {
    if (argc <= 1) {
	printf("usage:\n%s polynomial\n", argv[0]);
	return -1;
    }
    uint64_t mag = UINT64_C(12157665459056928801);
    GF2X poly1;
    GF2X jump;
    read_pols(poly1, argv[1]);
    uint32_t array[4];
    poly2array(array, poly1);
    pritarray(array);
    cout << endl;
    ZZ step;
    step = static_cast<uint32_t>(mag >> 32);
    step <<= 32;
    step += static_cast<uint32_t>(mag & 0xffffffffU);
    for (int i = 0; i < 32; i++) {
	PowerXMod(jump, step, poly1);
	poly2array(array, jump);
	pritarray(array);
	step <<= 1;
    }
}
