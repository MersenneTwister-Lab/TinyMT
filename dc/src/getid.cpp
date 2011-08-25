/**
 * @file getid.cpp
 *
 * @brief A tool program to check the output of dc programs and to get
 * the information to resume parameter generation.
 *
 * The Executable program made of this file is a tool programs to check
 * the output of dc programs and to get the information to resume parameter
 * generation.
 * Users can change this file to fit for their purpose without serious
 * influence on other parts.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */
#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <inttypes.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>

using namespace std;

namespace {
    bool bit32 = true;
    bool is_reverse = false;
    uint64_t p1, p2;
}

/**
 * parsing command line arguments
 * @param argc number of arguments
 * @param argv command line argument strings
 * @return are there any errors in arguments?
 */
bool parse_opt(int argc, char **argv) {
    int c;
    bool error = false;
    string pgm = argv[0];
    int bit;
    static struct option longopts[] = {
	{"bit", required_argument, NULL, 'b'},
	{"is_reverse", no_argument, NULL, 'r'},
	{NULL, 0, NULL, 0}};
    for (;;) {
	c = getopt_long(argc, argv, "b:r", longopts, NULL);
	if (error) {
	    break;
	}
	if (c == -1) {
	    break;
	}
	switch (c) {
	case 'r':
	    ::is_reverse = true;
	    break;
	case 'b':
	    bit = strtol(optarg, NULL, 10);
	    if (errno || (bit != 32 && bit != 64)) {
		error = true;
		cerr << "bit must be 32 or 64" << endl;
	    }
	    if (bit == 64) {
		bit32 = false;
	    } else {
		bit32 = true;
	    }
	    break;
	case '?':
	default:
	    error = true;
	    break;
	}
    }
    argc -= optind;
    argv += optind;
    if (argc < 1) {
	error = true;
    } else if (is_reverse && (argc < 2)) {
	error = true;
    }
    if (!error) {
	if (is_reverse) {
	    p1 = strtoull(argv[0], NULL, 0);
	    if (errno) {
		error = true;
		cerr << "id must be a number" << endl;
	    }
	} else {
	    p1 = strtoull(argv[0], NULL, 16);
	    if (errno) {
		error = true;
		cerr << "mat1 must be a hex number" << endl;
	    }
	}
    }
    if (!error && argc >= 2) {
	if (is_reverse) {
	    p2 = strtoull(argv[1], NULL, 0);
	    if (errno) {
		error = true;
		cerr << "seq must be a number" << endl;
	    }
	} else {
	    p2 = strtoull(argv[1], NULL, 16);
	    if (errno) {
		error = true;
		cerr << "mat2 must be a hex number" << endl;
	    }
	}
    }
    if (error) {
	cerr << pgm
	     << " -b 32 mat1 mat2" << endl;
	cerr << "calculate id and sequential number from mat1 and mat2" << endl;
	cerr << pgm
	     << " -b 64 mat1 mat2" << endl;
	cerr << "calculate id and sequential number from mat1 and mat2" << endl;
	cerr << pgm
	     << " -b 32 -r id seq" << endl;
	cerr << "calculate mat1 and mat2 form id and sequential number" << endl;
	cerr << pgm
	     << " -b 64 -r id seq" << endl;
	cerr << "calculate mat form id and sequential number" << endl;
	cerr << "numbers are base 10, or base 16 if they have prefix 0x"
	     << endl;
	return false;
    }
    return true;
}

/**
 * inverse function of shift and xor: f(x) = x ^ (x << shift)
 * @tparam T type of unsigned integer
 * @param x input of inverse function or the output of f(x)
 * @param shift shift value
 * @return the input of f(x)
 */
template<typename T>
T unshiftxor_left(T x, int shift) {
    T mask = 0;
    T work = x;
    mask = ~mask;
    for (int i = 0; i < static_cast<int>(sizeof(T) * 8 / shift); i++) {
	mask <<= shift;
	work &= ~mask;
	work = x ^ (work << shift);
    }
    return work;
}

/**
 * inverse function of shift and xor: f(x) = x ^ (x >> shift)
 * @tparam T type of unsigned integer
 * @param x input of inverse function or the output of f(x)
 * @param shift shift value
 * @return the input of f(x)
 */
template<typename T>
T unshiftxor_right(T x, int shift) {
    T mask = 0;
    T work = x;
    mask = ~mask;
    for (int i = 0; i < static_cast<int>(sizeof(T) * 8 / shift); i++) {
	mask >>= shift;
	work &= ~mask;
	work = x ^ (work >> shift);
    }
    return work;
}

/**
 * calculate id and internal sequence number from the output parameters
 * of tinymt32dc
 * @param id calculated id
 * @param seq calculated internal sequence number
 * @param mat1 recursion parameter 1
 * @param mat2 recursion parameter 2
 */
void calc_id32(uint32_t *id, uint32_t *seq, uint32_t mat1, uint32_t mat2) {
    uint32_t work;
    mat2 ^= 1;
    mat2 = unshiftxor_left<uint32_t>(mat2, 18);
    mat1 = unshiftxor_right<uint32_t>(mat1, 19);
    work = (mat2 & 0xffff) | (mat1 & 0xffff0000);
    *id = (mat2 & 0xffff0000) | (mat1 & 0xffff);
    work >>= 1;
    work = unshiftxor_left<uint32_t>(work, 15);
    work = unshiftxor_left<uint32_t>(work, 23);
    *seq = work & 0x7fffffff;
}

/**
 * calculate id and internal sequence number from the output parameters
 * of tinymt64dc
 * @param id calculated id
 * @param seq calculated internal sequence number
 * @param mat1 recursion parameter 1
 * @param mat2 recursion parameter 2
 */
void calc_id64(uint32_t *id, uint32_t *seq, uint32_t mat1, uint32_t mat2) {
    uint32_t work;
    mat2 = unshiftxor_left<uint32_t>(mat2, 18);
    mat1 = unshiftxor_right<uint32_t>(mat1, 19);
    work = (mat2 & 0xffff) | (mat1 & 0xffff0000);
    *id = (mat2 & 0xffff0000) | (mat1 & 0xffff);
    work = unshiftxor_left<uint32_t>(work, 15);
    work = unshiftxor_left<uint32_t>(work, 23);
    *seq = work;
}

/**
 * calculate recursion parameter mat1 and mat2 from id and internal sequence
 * number, like tinymt32dc.
 * CAUTION: this function does not check irreducibility of the characteristic
 * polynomial.
 * @param mat1 calculated recursion parameter 1
 * @param mat2 calculated recursion parameter 2
 * @param id 32-bit unsigned integer number given by user
 * @param seq internal sequence number
 */
void calc_mat32(uint32_t *mat1, uint32_t *mat2, uint32_t id, uint32_t seq) {
    uint32_t work = seq ^ (seq << 15) ^ (seq << 23);
    work <<= 1;
    *mat1 = (work & 0xffff0000) | (id & 0xffff);
    *mat2 = (work & 0xffff) | (id & 0xffff0000);
    *mat1 ^= *mat1 >> 19;
    *mat2 ^= *mat2 << 18;
    *mat2 ^= 1;
}

/**
 * calculate recursion parameter mat1 and mat2 from id and internal sequence
 * number, like tinymt64dc.
 * CAUTION: this function does not check irreducibility of the characteristic
 * polynomial.
 * @param id calculated id
 * @param seq calculated internal sequence number
 * @param mat1 recursion parameter 1
 * @param mat2 recursion parameter 2
 */
void calc_mat64(uint32_t *mat1, uint32_t *mat2, uint32_t id, uint32_t seq) {
    uint32_t work = seq ^ (seq << 15) ^ (seq << 23);
    *mat1 = (work & 0xffff0000) | (id & 0xffff);
    *mat2 = (work & 0xffff) | (id & 0xffff0000);
    *mat1 ^= *mat1 >> 19;
    *mat2 ^= *mat2 << 18;
}

/**
 * main function
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @return 0 if ended normally
 */
int main(int argc, char *argv[]) {
    if (!parse_opt(argc, argv)) {
	return -1;
    }
    if (bit32 && !::is_reverse) {
	uint32_t id;
	uint32_t seq;
	calc_id32(&id, &seq, p1, p2);
	cout << " id:" << dec << id << "(0x" << hex << id << ")" << endl;
	cout << "seq:" << dec << seq << "(0x" << hex << seq << ")" << endl;
    } else if (bit32 && ::is_reverse) {
	uint32_t mat1;
	uint32_t mat2;
	calc_mat32(&mat1, &mat2, p1, p2);
	cout << "mat1:0x" << hex << mat1 << endl;
	cout << "mat2:0x" << hex << mat2 << endl;
    } else if (!::is_reverse) {
	uint32_t id;
	uint32_t seq;
	calc_id64(&id, &seq, p1, p2);
	cout << " id:" << dec << id << "(0x" << hex << id << ")" << endl;
	cout << "seq:" << dec << seq << "(0x" << hex << seq << ")" << endl;
    } else {
	uint32_t mat1;
	uint32_t mat2;
	calc_mat64(&mat1, &mat2, p1, p2);
	cout << "mat1:0x" << hex << mat1 << endl;
	cout << "mat2:0x" << hex << mat2 << endl;
    }
    return 0;
}
