#ifndef SEARCH_ALL_HPP
#define SEARCH_ALL_HPP
/**
 * @file search_all.hpp
 *
 * @brief search irreducible polynomial and temper the output.
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
#include <unistd.h>
#include <time.h>
#include "recursion_search.hpp"
#include "random_util.hpp"
#include "simple_shortest_basis.hpp"
#include "search_temper.hpp"

/**
 * @namespace MTToolBox
 * name space for random number generator development
 */
namespace tinymt {
    /**
     * @class all_in_one
     * - search parameters of random number generator whose state transition
     * function has an irreducible characteristic polynomial.
     * - search tempering parameters for the generator.
     *
     * @tparam T type of generator's output, uint32_t or uint64_t.
     * @tparam G class of generator, always linear_generator with template.
     * @tparam ST tempering parameter searching strategy class.
     * @tparam STLSB tempering paramete searching strategy class for
     * tempering from LSB.
     * @tparam SG the sequential generator class
     */
    template<typename T, typename G, typename ST, typename STLSB, typename SG>
    class all_in_one {
    public:
	all_in_one(const G& ran, SG& p_mg) :
	    rand(ran),
	    mg(p_mg)
	    {
	    };
	/**
	 * search and temper
	 *
	 * @param lg linear generator class
	 * @param st tempering parameter searching strategy
	 * @param stlsb tempering parameter searching strategy for LSB.
	 * @param verbose verbose mode, output information about search process.
	 */
	bool search(G& lg, ST* st, STLSB* stlsb, bool verbose = true) {
	    using namespace MTToolBox;
	    using namespace NTL;
	    using namespace std;
	    using namespace std::tr1;

	    int veq[bit_size(T)];
	    Search<G, SG> s(lg, mg);
	    int mexp = lg.get_mexp();
	    bool found = false;
	    for (int i = 0;; i++) {
		if (s.start(10000)) {
		    found = true;
		    break;
		}
		if (verbose) {
		    cout << "not found in " << (i + 1) * 10000 << endl;
		}
	    }
	    if (!found) {
		return false;
	    }
	    if (verbose) {
		time_t t = time(NULL);
		cout << "irreducible parameter is found at " << ctime(&t);
	    }
	    rand = s.get_random();
	    if (verbose) {
		cout << "count = " << s.get_count() << endl;
		rand.out_param(cout);
	    }
	    poly = s.get_minpoly();
	    weight = NTL::weight(poly);
	    if (verbose) {
		shortest_basis<G, T> sb(rand, bit_size(T));
		print_binary(cout, poly);
		int delta = sb.get_all_equidist(veq);
		print_kv(veq, mexp, bit_size(T));
		cout << "delta = " << dec << delta << endl;
	    }
	    if (stlsb != NULL) {
		(*stlsb)(rand, verbose);
		if (verbose) {
		    rand.set_reverse_bit();
		    shortest_basis<G, T> sc(rand, bit_size(T));
		    delta = sc.get_all_equidist(veq);
		    rand.reset_reverse_bit();
		    time_t t = time(NULL);
		    cout << "lsb tempering parameters are found at "
			 << ctime(&t) << endl;
		    print_kv(veq, mexp, bit_size(T));
		    cout << "lsb delta = " << dec << delta << endl;
		}
	    }
	    (*st)(rand, verbose);
	    shortest_basis<G, T> sc(rand, bit_size(T));
	    delta = sc.get_all_equidist(veq);
	    if (verbose) {
		time_t t = time(NULL);
		cout << "tempering parameters are found at " << ctime(&t)
		     << endl;
		rand.out_param(cout);
		print_kv(veq, mexp, bit_size(T));
		cout << "delta = " << dec << delta << endl;
	    }
	    return true;
	};
	/** getter of rand */
	G& get_rand() {
	    return rand;
	};
	/** getter of weight */
	int get_weight() {
	    return weight;
	};
	/** getter of delta */
	int get_delta() {
	    return delta;
	};
	/** getter of the characteristic polynomial */
	NTL::GF2X get_characteristic_polynomial() {
	    return poly;
	};
	/** debug function */
	void print_kv(int veq[], int mexp, int size) {
	    using namespace std;
	    for (int i = 0; i < size; i++) {
		cout << dec << i + 1 << ":" << veq[i]
		     << "(" << mexp / (i + 1) - veq[i] << ")"
		     << endl;
	    }
	};

    private:
	/**
	 * the humming weight of the charateristic polynomial.
	 * e.g the number of terms of characteristic polynomial.
	 */
	int weight;
	/**
	 * sum of d(v) for all \b v.
	 */
	int delta;
	/**
	 * searched generator with its parameters.
	 */
	G rand;
	/**
	 * the characteristic polynomial of linear transition of the
	 * internal state.
	 */
	NTL::GF2X poly;
	/**
	 *
	 */
	SG mg;
    };

}
#endif
