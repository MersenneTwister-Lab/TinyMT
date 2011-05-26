#ifndef RANDOM_UTIL_HPP
#define RANDOM_UTIL_HPP
/**
 * @file random_util.hpp
 *
 * @brief some functions commonly used in MTToolBox.
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
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <inttypes.h>
#include <stdint.h>
#include <stdexcept>
#include <openssl/sha.h>
#include <tr1/memory>

#include <NTL/GF2X.h>

#define bit_size(tp) (static_cast<int>(sizeof(tp) * 8))

namespace MTToolBox {
    inline static int count_bit(uint16_t x);
    inline static int count_bit(uint32_t x);
    inline static int count_bit(uint64_t x);
    inline static uint32_t reverse_bit(uint32_t x);
    inline static uint64_t reverse_bit(uint64_t x);

    /**
     * calculate the largest number which is 2^m and does not exceed n.
     * @tparam T type of integer
     * @param n number
     * @returns the largest number which is 2^m and does not exceed n.
     */
    template<typename T>
    T floor2p(T n) {
	if (n == 1) {
	    return 1;
	} else {
	    return 2 * floor2p<T>(n / 2);
	}
    }

    /**
     * print polynomial in binary form. The coefficient of the smallest degree
     * is printed first.
     * @param os output-stream
     * @param poly polynomial to be printed
     * @param breakline if true, break line every 32 outputs.
     */
    inline static void print_binary(std::ostream& os,
				    NTL::GF2X& poly,
				    bool breakline = true) {
	using namespace NTL;
	if (deg(poly) < 0) {
	    os << "0deg=-1" << std::endl;
	    return;
	}
	for(int i = 0; i <= deg(poly); i++) {
	    if(rep(coeff(poly, i)) == 1) {
		os << '1';
	    } else {
		os << '0';
	    }
	    if (breakline && ((i % 32) == 31)) {
		os << std::endl;
	    }
	}
	os << "deg=" << deg(poly) << std::endl;
    }

    /**
     * change \b input to the number between \b start and \b end
     * @param input input number
     * @param start start of the range
     * @param end end of the range
     * @return the number r such that \b start <= \b r <= \b end.
     */
    inline static uint32_t get_range(uint32_t input, int start, int end) {
	if (end < start) {
	    printf("get_range:%d, %d\n", start, end);
	    exit(0);
	}
	return input % (end - start + 1) + start;
    }

    /**
     * change \b input to the number between \b start and \b end
     * @param input input number
     * @param start start of the range
     * @param end end of the range
     * @return the number r such that \b start <= \b r <= \b end.
     */
    inline static uint64_t get_range(uint64_t input, int start, int end) {
	if (end < start) {
	    printf("get_range:%u, %u\n", start, end);
	    exit(0);
	}
	return input % (end - start + 1) + start;
    }

    /**
     * change the small F2 table to the fast and redundant table.
     * @tparam T type of table member.
     * @param dist_tbl new redundant table
     * @param src_tbl source table
     * @param size size of \b dist_table
     */
    template<typename T>
    void fill_table(T dist_tbl[], T src_tbl[], int size) {
    	for(int i = 1; i < size; i++) {
	    for(int j = 1, k = 0; j <= i; j <<= 1, k++) {
		if (i & j) {
		    dist_tbl[i] ^= src_tbl[k];
		}
	    }
	}
    }

    /**
     * calculate the SHA1 digest of F2 polynomial. The coefficients of
     * the polynomial are changed to the string of "0" and "1", which
     * starts with the coefficient of the lowest degree, and the SHA1
     * hash of the string is calculated. The result hash is returned
     * by hexadecimal string.
     *
     * @param str output string
     * @param poly F2 polynomial
     */
    inline static void poly_sha1(std::string& str, const NTL::GF2X& poly) {
	using namespace NTL;
	using namespace std;
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	if (deg(poly) < 0) {
	    SHA1_Update(&ctx, "-1", 2);
	}
	for(int i = 0; i <= deg(poly); i++) {
	    if(rep(coeff(poly, i)) == 1) {
		SHA1_Update(&ctx, "1", 1);
	    } else {
		SHA1_Update(&ctx, "0", 1);
	    }
	}
	unsigned char md[SHA_DIGEST_LENGTH];
	SHA1_Final(md, &ctx);
	stringstream ss;
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
	    ss << setfill('0') << setw(2) << hex
	       << static_cast<int>(md[i]);
	}
	ss >> str;
    }


    /**
     * calculate the position of most right 1, or
     * least significant 1. The position of the MSB is
     * 0. returns -1 when \b v is zero.
     * citing from a website http://aggregate.org/MAGIC/#Trailing Zero Count
     *
     * @param x input
     * @return the position of most right 1.
     */
    int calc_1pos(uint16_t x)
    {
	if (x == 0) {
	    return -1;
	}
	int16_t y = (int16_t)x;
	y = count_bit((uint16_t)((y & -y) - 1));
	return 15 - y;
    }

    /**
     * calculate the position of most right 1, or
     * least significant 1. The position of the MSB is
     * 0. returns -1 when \b v is zero.
     * citing from a website http://aggregate.org/MAGIC/#Trailing Zero Count
     *
     * @param x input
     * @return the position of most right 1.
     */
    int calc_1pos(uint32_t x)
    {
	if (x == 0) {
	    return -1;
	}
	int32_t y = (int32_t)x;
	y = count_bit((uint32_t)(y & -y) - 1);
	return 31 - y;
    }

    /**
     * calculate the position of most right 1, or
     * least significant 1. The position of the MSB is
     * 0. returns -1 when \b v is zero.
     * citing from a website http://aggregate.org/MAGIC/#Trailing Zero Count
     *
     * @param x input
     * @return the position of most right 1.
     */
    int calc_1pos(uint64_t x)
    {
	if (x == 0) {
	    return -1;
	}
	int64_t y = (int64_t)x;
	y = count_bit((uint64_t)(y & -y) - 1);
	return 63 - y;
    }

    /**
     * check if \b array is all zero or not.
     * @tparam type of members of \b array
     * @param array checked array
     * @param size size of \b array
     * @return true if all elements of \b array are zero.
     */
    template<typename T>
    bool is_zero_array(T *array, int size) {
	if (array[0] != 0) {
	    return false;
	} else {
	    return (memcmp(array, array + 1, sizeof(T) * (size - 1)) == 0);
	}
    }

    /**
     * calculate the minimal polynomial of the generated sequence.
     * @returns the minimal polynomial
     */
    template<typename G>
    std::tr1::shared_ptr<NTL::GF2X> get_minpoly(G& generator, int length) {
	using namespace std;
	using namespace NTL;
	using namespace std::tr1;

	vec_GF2 vec;
	vec.SetLength(length * 2);
	for (int i = 0; i < length * 2; i++) {
	    vec[i] = generator() & 1;
	}
	shared_ptr<GF2X> minpoly(new GF2X());
	MinPolySeq(*minpoly, vec, length);
	return minpoly;
    }

    /**
     * count the number of 1
     * SIMD within a Register algorithm
     * citing from a website http://aggregate.org/MAGIC/
     */
    inline static int count_bit(uint16_t x) {
        x -= (x >> 1) & UINT16_C(0x5555);
        x = ((x >> 2) & UINT16_C(0x3333)) + (x & UINT16_C(0x3333));
        x = ((x >> 4) + x) & UINT16_C(0x0f0f);
        x += (x >> 8);
        return (int)(x & 0x1f);
    }

    inline static int count_bit(uint32_t x) {
        x -= (x >> 1) & UINT32_C(0x55555555);
        x = ((x >> 2) & UINT32_C(0x33333333)) + (x & UINT32_C(0x33333333));
        x = ((x >> 4) + x) & UINT32_C(0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return (int)(x & 0x3f);
    }

    inline static int count_bit(uint64_t x) {
	x -= (x >> 1) & UINT64_C(0x5555555555555555);
	x = ((x >> 2) & UINT64_C(0x3333333333333333))
	    + (x & UINT64_C(0x3333333333333333));
	x = ((x >> 4) + x) & UINT64_C(0x0f0f0f0f0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	x += (x >> 32);
	return (int)(x & 0x7f);
    }

    inline static uint32_t reverse_bit(uint32_t x)
    {
	uint32_t y = 0x55555555;
	x = (((x >> 1) & y) | ((x & y) << 1));
	y = 0x33333333;
	x = (((x >> 2) & y) | ((x & y) << 2));
	y = 0x0f0f0f0f;
	x = (((x >> 4) & y) | ((x & y) << 4));
	y = 0x00ff00ff;
	x = (((x >> 8) & y) | ((x & y) << 8));
	return((x >> 16) | (x << 16));
    }

    inline static uint64_t reverse_bit(uint64_t x)
    {
	uint64_t y = UINT64_C(0x5555555555555555);
	x = (((x >> 1) & y) | ((x & y) << 1));
	y = UINT64_C(0x3333333333333333);
	x = (((x >> 2) & y) | ((x & y) << 2));
	y = UINT64_C(0x0f0f0f0f0f0f0f0f);
	x = (((x >> 4) & y) | ((x & y) << 4));
	y = UINT64_C(0x00ff00ff00ff00ff);
	x = (((x >> 8) & y) | ((x & y) << 8));
	y = UINT64_C(0x0000ffff0000ffff);
	x = (((x >> 16) & y) | ((x & y) << 16));
	return((x >> 32) | (x << 32));
    }

    /**
     * divide and ceil
     */
    inline static int div_ceil(int x, int y) {
	if (x % y == 0) {
	    return x / y;
	} else {
	    return x / y + 1;
	}
    }

    /**
     * polynomial to string
     */
    inline static void to_str(uint8_t * str, int size, NTL::GF2X& poly) {
	if (deg(poly) >= size * 8) {
	    std::cerr << "str size too small" << std::endl;
	    throw new std::out_of_range("str size too small");
	}
	for (int i = 0; i < size; i++) {
	    str[i] = 0;
	}
	int idx = 0;
	for (int i = 0; i < size; i++) {
	    uint8_t mask = 1;
	    for (int j = 0; j < 8; j++) {
		if (IsOne(coeff(poly, idx++))) {
		    str[i] |= mask;
		}
		mask <<= 1;
	    }
	}
    }
}
#endif
