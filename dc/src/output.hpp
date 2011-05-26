#ifndef OUTPUT_HPP
#define OUTPUT_HPP 1
/**
 * @file output.hpp
 *
 * @brief output the result of tinymt32dc and tinymt64dc using text format.
 *
 * Command line option parser for tinymt32dc and tinymt64dc.
 * Users can change this file so that it fits for their applications
 * and OS, without any serious influence on other programs.
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

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <random_util.hpp>
#include "parse_opt.h"

namespace tinymt {
    template<typename P>
    static void output_params_stream(NTL::GF2X& poly,
				     int weight,
				     int delta,
				     int bit_size,
				     P& param,
				     bool first,
				     std::ostream& ost);
    void to_uint64(uint64_t *p1, uint64_t *p2, NTL::GF2X& poly);

    /**
     * output parameters to a file or standard output stream
     * @tparam T generator's output type, uint32_t or uint64_t
     * @tparam P parameter's class tinymt32_param or tinymt64_param
     * @param poly the characteristic polynomial of the generator
     * specified by the parameter.
     * @param weight Hamming weight of the characteristic polynomial
     * @param delta total dimension defect of equidistribution if tempering
     * function is GF(2)-linear.
     * @param param tinymt parameters
     * @param opt command line options
     * @param first first time call or not
     */
    template<typename T, typename P>
    void output_params(NTL::GF2X& poly,
		       int weight,
		       int delta,
		       P& param,
		       tinymt_options& opt,
		       bool first = true) {
	using namespace MTToolBox;
	using namespace std;

	int bit_size = sizeof(T) * 8;
	if (!opt.filename.empty()) {
	    // should check in parse param
	    ofstream ofs(opt.filename.c_str(), std::ios::out | std::ios::app);
	    if (ofs) {
		try {
		    output_params_stream<P>(poly, weight, delta, bit_size,
					    param, first, ofs);
		    ofs.close();
		} catch (...) {
		    ofs.close();
		}
	    }
	} else {
	    output_params_stream<P>(poly, weight, delta, bit_size,
				    param, first, cout);
	}
    }

    /**
     * output parameters to a file or standard output stream
     * @tparam T parameter's class tinymt32_param or tinymt64_param
     * @param poly the characteristic polynomial of the generator
     * specified by the parameter.
     * @param weight Hamming weight of the characteristic polynomial
     * @param delta total dimension defect of equidistribution if tempering
     * function is GF(2)-linear.
     * @param bit_size generator's output bit size, 32 or 64
     * @param param tinymt_parameters
     * @param first first time call or not
     * @param ost output stream
     */
    template<typename T>
    static void output_params_stream(NTL::GF2X& poly,
				     int weight,
				     int delta,
				     int bit_size,
				     T& param,
				     bool first,
				     std::ostream& ost) {
	using namespace MTToolBox;
	using namespace std;

	if (first) {
	    ost << "# charactristic, type, ";
	    ost << param.get_header();
	    ost	<< ", weight, delta" << endl;
	}
	ost << hex;
	uint64_t p1, p2;
	to_uint64(&p1, &p2, poly);
	ost << hex << setw(16) <<  setfill('0') << p1;
	ost << hex << setw(16) <<  setfill('0') << p2;
	ost << ",";
	ost << dec;
	ost << bit_size << ",";
	ost << param.get_string();
	ost << dec;
	ost << weight << ",";
	ost << delta;
	ost << endl;
    }

    /**
     * change polynomial of degree 127 to two 64-bit integers
     * usually polynomials are printed from low degree to high degree
     * and integers are printed from MSB to LSB.
     * In this function, and next printing, polynomials are printed
     * from high degree.
     * @param p1 high degree part
     * @param p2 low degree part
     * @param poly input polynomial
     */
    void to_uint64(uint64_t *p1, uint64_t *p2, NTL::GF2X& poly) {
	uint64_t p = 0;
	uint64_t mask = 1;
	for(int i = 0; i < 64; i++) {
	    if (IsOne(coeff(poly, i))) {
		p |= mask;
	    }
	    mask <<= 1;
	}
	*p2 = p;
	mask = 1;
	p = 0;
	for(int i = 0; i < 64; i++) {
	    if (IsOne(coeff(poly, i + 64))) {
		p |= mask;
	    }
	    mask <<= 1;
	}
	*p1 = p;
    }

}
#endif
