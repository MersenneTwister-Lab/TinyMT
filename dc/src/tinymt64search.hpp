#ifndef TINYMT64SEARCH_HPP
#define TINYMT64SEARCH_HPP
/**
 * @file tinymt64search.hpp
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state,
 * this class is used by tinymt32dc.
 *
 * This file is important. Users should not change this file,
 * except they are experts in random number generation.
 * This file is used for parameter searching.
 * For random number generation, use tinymt64c.c and tinymt64c.h.
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
#include <NTL/GF2X.h>
#include <search_temper.hpp>

namespace tinymt {
    using namespace MTToolBox;
    using namespace NTL;
    using namespace std;

    class tinymt64;
    //typedef search_temper<tinymt64, uint64_t, 64, 1, 32, 9> st64;
    /**
     * tempering parameter search algorithm
     * - generator is tinymt64
     * - generator's output type is uint64_t
     * - bits in output is 64
     * - size of tempering parameter array is 1
     * - tempering 52 bit from MSB
     * - tempering parameter is searched every 6 bits
     */

    typedef search_temper<tinymt64, uint64_t, 64, 1, 52, 6> st64;
    /**
     * tempering parameter search algorithm
     * - generator is tinymt64
     * - generator's output type is uint64_t
     * - bits in output is 64
     * - size of tempering parameter array is 1
     * - tempering 12 bit from LSB
     * - tempering parameter is searched every 6 bits
     */
    typedef search_temper<tinymt64, uint64_t, 64, 1, 12, 6, true> stlsb64;

    /**
     * @class tinymt64_param
     * @brief a class keeping parameters of tinymt64
     *
     * This class keeps parameters of tinymt64, and has some
     * method for outputting parameters.
     */
    class tinymt64_param {
    public:
	uint32_t id;
	uint32_t mat1;
	uint32_t mat2;
	uint64_t tmat[1];
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
	    ss << hex << setw(16) << setfill('0') << tmat[0] << ",";
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
	    ss << "tmat:" << hex << setw(16) << setfill('0') << tmat[0] << endl;
	    string s;
	    ss >> s;
	    return s;
	};
    };

    /**
     * @class tinymt64
     * @brief 64-bit tinymt generator class used for dynamic creation
     *
     * This class is one of the main class of tinymt dynamic creator.
     * This class is designed to be called from programs in MTToolBox,
     * but is not a subclass of some abstract class.
     * Instead, this class is passed to them as template parameters.
     */
    class tinymt64 {
    public:
	/**
	 * Constructor by id.
	 * @param id The number specified by a user.
	 */
	tinymt64(uint32_t id) {
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
	tinymt64(const tinymt64& src) :
	    param(src.param) {
	    reverse_bit_flag = false;
	};

	/**
	 * Constructor by parameter.
	 * @param src_param
	 */
	tinymt64(const tinymt64_param& src_param) : param(src_param) {
	    reverse_bit_flag = false;
	};

	/**
	 * This method is called by the functions in simple_shortest_basis.hpp
	 */
	void make_zero_status() {
	    status[0] = 0;
	    status[1] = 0;
	};

	/**
	 * This method always returns 127
	 * @return always 127
	 */
	int get_mexp() const {
	    return mexp;
	};

	/**
	 * This method always returns 2
	 * @return always 2
	 */
	int get_status_size() const {
	    return status_size;
	};

	/**
	 * This method initialize internal state.
	 * This initialization is simple.
	 * @param seed seed for initialization
	 */
	void seeding(uint64_t seed) {
	    status[0] = 0;
	    status[1] = seed;
	};

	/**
	 * Important state transition function.
	 */
	void next_state() {
	    uint64_t x;
	    status[0] &= UINT64_C(0x7fffffffffffffff);
	    x = status[0] ^ status[1];
	    x ^= x << sh0;
	    x ^= x >> 32;
	    x ^= x << 32;
	    x ^= x << sh1;
	    status[0] = status[1];
	    status[1] = x;
	    if (x & 1) {
		status[0] ^= static_cast<uint64_t>(param.mat1);
		status[1] ^= static_cast<uint64_t>(param.mat2) << 32;
	    }
	};

	/**
	 * get a part of internal state without tempering
	 * @return
	 */
	uint64_t get_uint() {
	    return status[0];
	};

	/**
	 * getter of status
	 * @param index index of internal status
	 * @return element of internal status at \b index
	 */
	uint64_t get_status(int index) {
	    return status[index];
	};

	/**
	 * setter of status
	 * @param value new status value
	 */
	void set_status(uint64_t value[2]) {
	    status[0] = value[0];
	    status[1] = value[1];
	};

	/**
	 * Important method, generate new random number
	 * @return new pseudo random number
	 */
	uint64_t generate() {
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
	uint64_t generate(int bit_len) {
	    uint64_t u;
	    if (reverse_bit_flag) {
		u = reverse_bit(generate());
	    } else {
		u = generate();
	    }
	    uint64_t mask = 0;
	    mask = (~mask) << (64 - bit_len);
	    return u & mask;
	};

	/**
	 * make parameters from given sequential number and
	 * internal id
	 * @param num sequential number
	 */
	void setup_param(uint32_t num) {
	    uint32_t work = num ^ (num << 15) ^ (num << 23);
	    param.mat1 = (work & 0xffff0000) | (param.id & 0xffff);
	    param.mat2 = (work & 0xffff) | (param.id & 0xffff0000);
	    param.mat1 ^= param.mat1 >> 19;
	    param.mat2 ^= param.mat2 << 18;
	    param.tmat[0] = 0;
	};

	/**
	 * getter of parameter
	 * @return parameter
	 */
	const tinymt64_param& get_param() const {
	    return param;
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
	    return (status[0] == 0) && (status[1] == 0);
	};

	/**
	 * This is important,
	 * This function tries to improve output quality of randomness.
	 * One important point of tinymt is this tempering function,
	 * which is not GF(2)-linear.
	 * But in calculating parameter phase, NON_LINEAR is never defined.
	 * @return improved random number
	 */
	uint64_t temper() {
#if defined(NO_TEMPER)
	    return status[0];
#else
	    uint64_t x;
#if defined(NON_LINEAR)
	    x = status[0] + status[1];
#else
	    x = status[0] ^ status[1];
#endif
	    x ^= status[0] >> sh8;
	    if (x & 1) {
		x ^= param.tmat[0];
	    }
	    return x;
#endif
	};

	/**
	 * This method is called by functions in the file search_temper.hpp
	 * @param mask available bits of pattern
	 * @param pattern bit pattern
	 * @param src_bit only 0 is allowed
	 */
        void set_temper_pattern(uint64_t mask, uint64_t pattern, int src_bit) {
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
	void add(tinymt64& that) {
	    status[0] ^= that.status[0];
	    status[1] ^= that.status[1];
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
	void set_param(const tinymt64_param& src) {
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
	enum {sh0 = 12, sh1 = 11, sh8=8, status_size = 4, mexp = 127};
	uint64_t status[2];
	tinymt64_param param;
	bool reverse_bit_flag;
    };
}

#endif
