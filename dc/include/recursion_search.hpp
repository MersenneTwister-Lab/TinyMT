#ifndef RECURSION_SEARCH_HPP
#define RECURSION_SEARCH_HPP
/**
 * @file recursion_search.hpp
 *
 * @brief search parameters so that the random number generator's state
 * transition function has an irreducible characteristic polynomial.
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
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>

namespace tinymt {
    /**
     * @class Search
     * search parameters so that the generator's state transition function
     * has an irreducible characteristic polynomial.
     * 1) call start() function.
     * 2) if start() returns true, then call get_random(), get_minpoly(),
     * or get_count().
     *
     * @tparam T generators class
     * @tparam SG sequential generator
     */
    template<class T, class SG> class Search {
    public:
	void get_minpoly(NTL::GF2X& minpoly, T& rand) {
	    using namespace std;
	    using namespace NTL;

	    vec_GF2 vec;
	    int mexp = rand.get_mexp();
	    vec.SetLength(mexp * 2);
	    for (int i = 0; i < mexp * 2; i++) {
		vec[i] = rand.generate() & 1;
	    }
	    MinPolySeq(minpoly, vec, mexp);
	};

	/**
	 * generate random parameters and check if the generator's
	 * state transition function has an irreducible characteristic
	 * polynomial. If found in \b try_count times, return true, else
	 * return false.
	 *
	 * @param try_count
	 */
	bool start(int try_count) {
	    long mexp = rand.get_mexp();
	    long degree;
	    for (int i = 0; i < try_count; i++) {
		rand.setup_param(sg->next());
		rand.seeding(1);
		get_minpoly(minpoly, rand);
		count++;
		degree = deg(minpoly);
		if (degree != mexp) {
		    continue;
		}
		if (IterIrredTest(minpoly)) {
		    return true;
		}
	    }
	    return false;
	};
	/**
	 * call this function after \b start() has returned true.
	 * @return random number generator class with parameters.
	 */
	const T& get_random() const {
	    return rand;
	};

	/**
	 * call this function after \b start() has returned true.
	 * In this program, if minimal polynomial is irreducible,
	 * then the polynomial is characteristic polynomial of
	 * generator's state transition function.
	 *
	 * @return minimal polynomial of generated sequence.
	 */
	const NTL::GF2X& get_minpoly() const {
	    return minpoly;
	};

	/**
	 * @return tried count after this class has created.
	 */
	int get_count() const {
	    return count;
	};

	/**
	 * @param rand_ random number generator whose parameters are
	 * searched.
	 * @param seq_generator a sequential number generator which
	 * gives sequential number for searching parameters.
	 *
	 */
	Search(const T& rand_, SG& seq_generator) :
	    rand(rand_)
	    {
		sg = &seq_generator;
		count = 0;
	    };
    private:
	T rand;
	NTL::GF2X minpoly;
	SG *sg;
	int count;
    };
}
#endif
