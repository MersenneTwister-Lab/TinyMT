/**
 * @file test64.cl
 *
 * @brief Test Program for openCL 1.2
 *
 * tinymt64
 * This program generates 64-bit unsigned integers.
 * The period of generated integers is 2<sup>127</sup>-1.
 *
 * This also generates double precision floating point numbers
 * uniformly distributed in the range [1, 2). (float r; 1.0 <= r < 2.0)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2013 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include "tinymt64.clh"

/**
 * kernel function.
 * This function initialize internal state of tinymt64.
 *
 * @param d_status internal state of kernel side tinymt
 * @param seed seed of initialization
 */
__kernel void
tinymt_init_seed_kernel(__global tinymt64wp_t * d_status,
			ulong seed)
{
    const size_t id = tinymt_get_sequential_id();
    tinymt64wp_t tiny;

    tinymt64_status_read(&tiny, d_status);
    tinymt64_init(&tiny, seed + id);
    tinymt64_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function initialize internal state of tinymt64.
 *
 * @param d_status internal state of kernel side tinymt
 * @param seeds seeds of initialization
 * @param length length of seeds
 */
__kernel void
tinymt_init_array_kernel(__global tinymt64wp_t * d_status,
			 __global ulong * seeds,
			int length)
{
    const size_t id = tinymt_get_sequential_id();
    tinymt64wp_t tiny;
    ulong local_seeds[10];
    if (length > 10) {
	length = 10;
    }
    for (int i = 0; i < length; i++) {
	local_seeds[i] = seeds[i];
    }
    tinymt64_status_read(&tiny, d_status);
    tinymt64_init_by_array(&tiny, local_seeds, length);
    tinymt64_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function generates 64-bit unsigned integers in d_data
 *
 * @param d_status internal state of kernel side tinymt
 * @param d_data output
 * @param size number of output data requested.
 */
__kernel void
tinymt_uint64_kernel(__global tinymt64wp_t * d_status,
		     __global ulong * d_data,
		     int size)
{
    const size_t id = tinymt_get_sequential_id();
    const size_t global_size = tinymt_get_sequential_size();
    tinymt64wp_t tiny;

    tinymt64_status_read(&tiny, d_status);
    for (int i = 0; i < size; i++) {
	d_data[global_size * i + id] = tinymt64_uint64(&tiny);
    }
    tinymt64_status_write(d_status, &tiny);
}

#if defined(HAVE_DOUBLE)
/**
 * kernel function.
 * This function generates double floats in the range [1,2) in d_data
 *
 * @param d_status internal state of kernel side tinymt
 * @param d_data output
 * @param size number of output data requested.
 */
__kernel void
tinymt_double12_kernel(__global tinymt64wp_t * d_status,
		       __global double * d_data,
		       int size)
{
    const size_t id = tinymt_get_sequential_id();
    const size_t sequential_size = tinymt_get_sequential_size();
    tinymt64wp_t tiny;

    tinymt64_status_read(&tiny, d_status);
    for (int i = 0; i < size; i++) {
	d_data[sequential_size * i + id] = tinymt64_double12(&tiny);
    }
    tinymt64_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function generates double float in the range [0,1) in d_data
 *
 * @param d_status internal state of kernel side tinymt
 * @param d_data output
 * @param size number of output data requested.
 */
__kernel void
tinymt_double01_kernel(__global tinymt64wp_t * d_status,
		       __global double * d_data,
		       int size)
{
    const size_t id = tinymt_get_sequential_id();
    const size_t sequential_size = tinymt_get_sequential_size();
    tinymt64wp_t tiny;

    tinymt64_status_read(&tiny, d_status);
    for (int i = 0; i < size; i++) {
	d_data[sequential_size * i + id] = tinymt64_double01(&tiny);
    }
    tinymt64_status_write(d_status, &tiny);
}
#endif
