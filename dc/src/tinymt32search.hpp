#ifndef TINYMT32SEARCH_HPP
#define TINYMT32SEARCH_HPP
/**
 * @file tinymt32search.hpp
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state,
 * this class is used by tinymt32dc.
 *
 * This file is important. Users should not change this file,
 * except they are experts in random number generation.
 * This file is used for parameter searching.
 * For random number generation, use tinymt32c.c and tinymt32c.h.
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
#include <cstdlib>
#include <cerrno>
#include <sstream>
#include <unistd.h>
#include <search_temper.hpp>

/**
 * @namespace tinymt
 * name space for tinymt32 and tinymt64
 */
namespace tinymt {
    using namespace MTToolBox;
    using namespace NTL;
    using namespace std;

    class tinymt32;
    /**
     * Tempering parameter searching algorithm.
     * - Tempering for tinymt32.
     * - The output of pseudo random number generator is 32 bits.
     * - Tempering parameter is one unsigned 32-bit integer.
     * - Tempering 23 bits from MSB.
     * - Searching parameters by 6 bits at once.
     */
    typedef search_temper<tinymt32, uint32_t, 32, 1, 23, 6> st32;

    /**
     * - Tempering parameter searching algorithm.
     * - Tempering for tinymt32.
     * - The output of pseudo random number generator is 32 bits.
     * - Tempering parameter is one unsigned 32-bit integer.
     * - Tempering 9 bits from LSB.
     * - Searching parameters by 5 bits at once.
     */
    typedef search_temper<tinymt32, uint32_t, 32, 1, 9, 5, true> stlsb32;

    /**
     * @class tinymt32_param
     * @brief a class keeping parameters of tinymt32
     *
     * This class keeps parameters of tinymt32, and has some
     * method for outputting parameters.
     */
    class tinymt32_param {
    public:
	uint32_t id;
	uint32_t mat1;
	uint32_t mat2;
	uint32_t tmat[1];
	/**
	 * This method is used in output.hpp.
	 * @return header line of output.
	 */
	string get_header() {
	    return "id, mat1, mat2, tmat";
	};

	/**
	 * This method is used in output.hpp.
	 * @return string of parameters
	 */
	string get_string() {
	    stringstream ss;
	    ss << dec << id << ",";
	    ss << hex << setw(8) << setfill('0') << mat1 << ",";
	    ss << hex << setw(8) << setfill('0') << mat2 << ",";
	    ss << hex << setw(8) << setfill('0') << tmat[0] << ",";
	    string s;
	    ss >> s;
	    return s;
	};

	/**
	 * This method is used for DEBUG.
	 * @return string of parameters.
	 */
	string get_debug_string() {
	    stringstream ss;
	    ss << "id:" << dec << id << endl;
	    ss << "mat1:" << hex << setw(8) << setfill('0') << mat1 << endl;
	    ss << "mat2:" << hex << setw(8) << setfill('0') << mat2 << endl;
	    ss << "tmat:" << hex << setw(8) << setfill('0') << tmat[0] << endl;
	    string s;
	    ss >> s;
	    return s;
	};
	/**
	 * This method writes parameters to output stream.
	 * @param out The output stream to which parameters are written.
	 */
	void binary_write(ostream& out) {
	    out.write((char *)&mat1, sizeof(uint32_t));
	    out.write((char *)&mat2, sizeof(uint32_t));
	    out.write((char *)&tmat[0], sizeof(uint32_t));
	};
    };

    /**
     * @class tinymt32
     * @brief 32-bit tinymt generator class used for dynamic creation
     *
     * This class is one of the main class of tinymt dynamic creator.
     * This class is designed to be called from programs in MTToolBox,
     * but is not a subclass of some abstract class.
     * Instead, this class is passed to them as template parameters.
     */
    class tinymt32 {
    public:
	/**
	 * Constructor by id.
	 * @param id The number specified by a user.
	 */
	tinymt32(uint32_t id) {
	    param.id = id;
	    param.mat1 = 0;
	    param.mat2 = 0;
	    param.tmat[0] = 0;
	    reverse_bit_flag = false;
	};

	/**
	 * The copy constructor.
	 * @param src The origin of copy.
	 */
	tinymt32(const tinymt32& src) : param(src.param) {
	    reverse_bit_flag = false;
	};

	/**
	 * Constructor by parameter.
	 * @param src_param
	 */
	tinymt32(const tinymt32_param& src_param) : param(src_param) {
	    reverse_bit_flag = false;
	};

	/**
	 * This method is called by the functions in simple_shortest_basis.hpp
	 */
	void make_zero_status() {
	    status[0] = 0;
	    status[1] = 0;
	    status[2] = 0;
	    status[3] = 0;
	};

	/**
	 * This method always returns 127
	 * @return always 127
	 */
	int get_mexp() const {
	    return mexp;
	};

	/**
	 * This method always returns 4
	 * @return always 4
	 */
	int get_status_size() const {
	    return status_size;
	};

	/**
	 * This method initialize internal state.
	 * This initialization is simple.
	 * @param seed seed for initialization
	 */
	void seeding(uint32_t seed) {
	    status[0] = 0;
	    status[1] = 0;
	    status[2] = 0;
	    status[3] = seed;
	};

	/**
	 * Important state transition function.
	 */
	void next_state() {
	    uint32_t x;
	    uint32_t y;
	    y = status[3];
	    x = (status[0] & 0x7fffffff) ^ status[1] ^ status[2];
	    x ^= (x << sh0);
	    y ^= (y >> sh0) ^ x;
	    status[0] = status[1];
	    status[1] = status[2];
	    status[2] = x ^ (y << sh1);
	    status[3] = y;
	    if (y & 1) {
		status[1] ^= param.mat1;
		status[2] ^= param.mat2;
	    }
	};

	/**
	 * get a part of internal state without tempering
	 * @return
	 */
	uint32_t get_uint() {
	    return status[0];
	};

	/**
	 * getter of status
	 * @param index index of internal status
	 * @return element of internal status at \b index
	 */
	uint32_t get_status(int index) {
	    return status[index];
	};

	/**
	 * setter of status
	 * @param value new status value
	 */
	void set_status(uint32_t value[4]) {
	    for (int i = 0; i < 4; i++) {
		status[i] = value[i];
	    }
	};

	/**
	 * Important method, generate new random number
	 * @return new pseudo random number
	 */
	uint32_t generate() {
	    next_state();
	    return temper();
	};

	/**
	 * This method is called by the functions in simple_shortest_basis.hpp
	 * This method returns \b bit_len bits of MSB of generated numbers
	 * If reverse_bit_flag is set, bits are taken from LSB
	 * @param bit_len bit length from MSB or LSB
	 * @return generated numbers of bit_len
	 */
	uint32_t generate(int bit_len) {
	    uint32_t u;
	    if (reverse_bit_flag) {
		u = reverse_bit(generate());
	    } else {
		u = generate();
	    }
	    uint32_t mask = 0;
	    mask = (~mask) << (32 - bit_len);
	    return u & mask;
	};

	/**
	 * make parameters from given sequential number and
	 * internal id
	 * @param num sequential number
	 */
	void setup_param(uint32_t num) {
	    uint32_t work = num ^ (num << 15) ^ (num << 23);
	    work <<= 1;
	    param.mat1 = (work & 0xffff0000) | (param.id & 0xffff);
	    param.mat2 = (work & 0xffff) | (param.id & 0xffff0000);
	    param.mat1 ^= param.mat1 >> 19;
	    param.mat2 ^= param.mat2 << 18;
	    param.mat2 ^= 1;
	    param.tmat[0] = 0;
	};

	/**
	 * getter of parameter
	 * @return parameter
	 */
	const tinymt32_param& get_param() const {
	    return param;
	};

	/**
	 * getter of recursion parameter
	 * @param mat1 parameter mat1 is set after calling this method
	 * @param mat2 parameter mat2 is set after calling this method
	 */
	void get_mat(uint32_t *mat1, uint32_t *mat2) const {
	    *mat1 = param.mat1;
	    *mat2 = param.mat2;
	};

	/**
	 * This method gives information to the functions in the file
	 * search_temper.hpp
	 * @return always 1
	 */
	int get_temper_param_num() const {
	    return 1;
	};

	/**
	 * This method is called by the functions in the file
	 * simple_shortest_basis.hpp
	 * @return true if all elements of status is zero
	 */
	bool is_zero() {
	    return (status[0] == 0) &&
		(status[1] == 0) &&
		(status[2] == 0) &&
		(status[3] == 0);
	};

	/**
	 * This is important,
	 * This function tries to improve output quality of randomness.
	 * One important point of tinymt is this tempering function,
	 * which is not GF(2)-linear.
	 * But in calculating parameter phase, NON_LINEAR is never defined.
	 * @return improved random number
	 */
	uint32_t temper() {
#if defined(NO_TEMPER)
	    return status[0];
#else
	    uint32_t t0, t1;
#if defined(NON_LINEAR)
	    t0 = status[3];
	    t1 = status[0] + (status[2] >> sh8);
#else
	    t0 = status[3];
	    t1 = status[0] ^ (status[2] >> sh8);
#endif
	    t0 ^= t1;
	    if (t1 & 1) {
		t0 ^= param.tmat[0];
	    }
	    return t0;
#endif
	};

	/**
	 * This method is called by functions in the file search_temper.hpp
	 * @param mask available bits of pattern
	 * @param pattern bit pattern
	 * @param src_bit only 0 is allowed
	 */
	void set_temper_pattern(uint32_t mask, uint32_t pattern, int src_bit) {
	    param.tmat[src_bit] &= ~mask;
	    param.tmat[src_bit] |= pattern & mask;
	};

	/**
	 * This method is called by functions in the file
	 * simple_shortest_basis.hpp addition of internal state as
	 * GF(2) vector is possible when state transition function and
	 * output function is GF(2)-linear.
	 * @param that tinymt generator added to this generator
	 */
	void add(tinymt32& that) {
	    status[0] ^= that.status[0];
	    status[1] ^= that.status[1];
	    status[2] ^= that.status[2];
	    status[3] ^= that.status[3];
	};

	/**
	 * This method is called by functions in the file search_temper.hpp
	 * Do not remove this.
	 */
	void setup_temper() {
	};

	/**
	 * setter of parameter
	 * @param src new parameter
	 */
	void set_param(tinymt32_param src) {
	    param = src;
	};

	/**
	 * output parameters
	 * @param out output stream
	 */
	void out_param(ostream& out) {
	    string s = param.get_debug_string();
	    out << s << endl;
	};

	/**
	 * This method is called by the functions in search_temper.hpp
	 * to calculate the equidistribution properties from LSB
	 */
	void set_reverse_bit() {
	    reverse_bit_flag = true;
	};

	/**
	 * This method is called by the functions in search_temper.hpp
	 * to reset the reverse_bit_flag
	 */
	void reset_reverse_bit() {
	    reverse_bit_flag = false;
	};
    private:
	enum {sh0 = 1, sh1 = 10, sh8=8, status_size = 4, mexp = 127};
	uint32_t status[4];
	tinymt32_param param;
	bool reverse_bit_flag;
    };
}

#endif
