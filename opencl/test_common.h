#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdint.h>
#include <inttypes.h>
#include <iostream>
#include <iomanip>

#define TINYMT32_MEXP 127
#define TINYMT32_MASK UINT32_C(0x7fffffff)

static inline void print_uint32(uint32_t data[], int size, int item_num)
{
    using namespace std;

    int max_seq = 10;
    int max_item = 6;
    if (size / item_num < max_seq) {
	max_seq = size / item_num;
    }
    if (item_num < max_item) {
	max_item = item_num;
    }
    for (int i = 0; i < max_seq; i++) {
	for (int j = 0; j < max_item; j++) {
	    cout << setw(10) << dec << data[item_num * i + j] << " ";
	}
	cout << endl;
    }
}

static inline void print_float(float data[], int size, int item_num)
{
    using namespace std;

    int max_seq = 10;
    int max_item = 6;
    if (size / item_num < max_seq) {
	max_seq = size / item_num;
    }
    if (item_num < max_item) {
	max_item = item_num;
    }
    for (int i = 0; i < max_seq; i++) {
	for (int j = 0; j < max_item; j++) {
	    cout << setprecision(9) << setw(12)
		 << dec << left << setfill(' ')
		 << data[item_num * i + j] << " ";
	}
	cout << endl;
    }
}

static inline void print_uint64(uint64_t data[], int size, int item_num)
{
    using namespace std;

    int max_seq = 10;
    int max_item = 3;
    if (size / item_num < max_seq) {
	max_seq = size / item_num;
    }
    if (item_num < max_item) {
	max_item = item_num;
    }
    for (int i = 0; i < max_seq; i++) {
	for (int j = 0; j < max_item; j++) {
	    cout << setw(20) << dec << data[item_num * i + j] << " ";
	}
	cout << endl;
    }
}

static inline void print_double(double data[], int size, int item_num)
{
    using namespace std;

    int max_seq = 10;
    int max_item = 3;
    if (size / item_num < max_seq) {
	max_seq = size / item_num;
    }
    if (item_num < max_item) {
	max_item = item_num;
    }
    for (int i = 0; i < max_seq; i++) {
	for (int j = 0; j < max_item; j++) {
	    cout << setprecision(18) << setw(21)
		 << dec << left << setfill(' ')
		 << data[item_num * i + j] << " ";
	}
	cout << endl;
    }
}

#endif
