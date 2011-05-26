#ifndef SEQUENTIAL_HPP
#define SEQUENTIAL_HPP
/**
 * @file sequential.hpp
 *
 * @brief Generate sequentially count down numbers.
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

#include <stdexcept>

namespace MTToolBox {
    /**
     * @class Sequential
     * @brief sequential count down generator
     *
     * - sequential count down generator
     * - This class is able to be used in place of random number generator
     *
     * @tparam T output data type, and internal counter type
     */
    template<typename T>
    class Sequential {
    public:
	Sequential(T p_mask) {
	    status = reinterpret_cast<T>(-1);
	    mask = p_mask;
	    error = false;
	};
	Sequential(T p_mask, T seed) {
	    status = seed;
	    mask = p_mask;
	    error = false;
	};
	Sequential(Sequential<T>& src) {
	    status = src.status;
	    mask = src.mask;
	    error = src.error;
	};
	void reseed(T seed) {
	    status = seed;
	    error = false;
	};
	T next() {
	    if (error) {
		throw std::underflow_error("count over zero exception");
	    }
	    if (status <= 0) {
		error = true;
	    }
	    T work = status;
	    status -= 1;
	    return work ^ mask;
	};
    private:
	T status;
	T mask;
	bool error;
    };
}
//  LocalWords:  namespace

#endif

