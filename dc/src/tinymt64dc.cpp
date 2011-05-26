/**
 * @file tinymt64dc.cpp
 *
 * @brief The main function of parameter generator of 64-bit Tiny
 * Mersenne Twister.
 *
 * The functions in this file are simple. They parse the command line
 * options and call all_in_one function which does almost all things.
 * Users can change this file so that it fits to their applications
 * and OS.
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
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
#include <search_all.hpp>
#include <sequential.hpp>
#include "tinymt64search.hpp"
#include "parse_opt.h"
#include "output.hpp"

using namespace std;
using namespace MTToolBox;
using namespace tinymt;

typedef Sequential<uint32_t> Seq32;
static const uint32_t sequence_max = 0xffffffff;

int search(tinymt_options& opt, int count);

int main(int argc, char** argv) {
    tinymt_options opt;
    bool parse = parse_opt(opt, argc, argv, sequence_max);
    if (!parse) {
	return -1;
    }
    try {
	return search(opt, opt.count);
    } catch (underflow_error e) {
	return 0;
    }
}

int search(tinymt_options& opt, int count) {
    Seq32 sq(0, opt.start);
    tinymt64 g(opt.uid);

    if (opt.verbose) {
	time_t t = time(NULL);
	cout << "search start at " << ctime(&t);
	cout << "id:" << dec << opt.uid << endl;
    }
    all_in_one<uint64_t, tinymt64, st64, stlsb64, Seq32> all(g, sq);
    st64 st;
    stlsb64 stlsb;
    for (int i = 0; i < count; i++) {
	if (all.search(g, &st, &stlsb, opt.verbose)) {
	    g = all.get_rand();
	    tinymt64_param param = g.get_param();
	    int weight = all.get_weight();
	    int delta = all.get_delta();
	    GF2X poly = all.get_characteristic_polynomial();
		output_params<uint64_t, tinymt64_param>(poly, weight,
							delta, param, opt,
							i == 0);
	} else {
	    cout << "search failed" << endl;
	    return -1;
	}
    }
    if (opt.verbose) {
	time_t t = time(NULL);
	cout << "search end at " << ctime(&t);
    }
    return 0;
}
