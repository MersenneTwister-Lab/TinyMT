/**
 * @file test32_jump2.cl
 *
 * @brief Test Program for openCL 1.2
 *
 * This program generates 32-bit unsigned integers.
 * The period of generated integers is 2<sup>127</sup>-1.
 *
 * This also generates single precision floating point numbers
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
#include "tinymt32_jump.clh"

/**
 * kernel function.
 * This function initialize internal state of tinymt32 and jump
 * using id of work item.
 *
 * @param d_status internal state of kernel side tinymt
 * @param jump_table table of jump polynomials
 * @param seed seed of initialization
 */
__kernel void
tinymt_init_seed_kernel(__global tinymt32j_t * d_status,
			__constant uint * jump_table,
			uint seed)
{
    tinymt32j_t tiny;
    size_t gid = tinymt_get_sequential_id();

    tinymt32j_init_seed(&tiny, seed);
    for (int i = 0; gid != 0; i++) {
        if ((gid & 1) != 0) {
            tinymt32j_jump_by_array(&tiny, &jump_table[i * 4]);
        }
        gid = gid >> 1;
    }
    tinymt32j_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function initialize internal state of tinymt32.
 *
 * @param d_status internal state of kernel side tinymt
 * @param jump_array a jump polynomial
 */
__kernel void
tinymt_jump_kernel(__global tinymt32j_t * d_status,
		   __constant uint * jump_array)
{
    tinymt32j_t tiny;
    size_t gid = tinymt_get_sequential_id();

    tinymt32j_status_read(&tiny, d_status);
    tinymt32j_jump_by_array(&tiny, jump_array);
    tinymt32j_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function generates 32-bit unsigned integers in d_data
 *
 * @param d_status internal state of kernel side tinymt
 * @param d_data output
 * @param size number of output data requested.
 */
__kernel void
tinymt_uint32_kernel(__global tinymt32j_t * d_status,
		     __global uint* d_data,
		     int size)
{
    const size_t id = tinymt_get_sequential_id();
    const size_t global_size = tinymt_get_sequential_size();
    tinymt32j_t tiny;

    tinymt32j_status_read(&tiny, d_status);
    for (int i = 0; i < size; i++) {
	d_data[size * id + i] = tinymt32j_uint32(&tiny);
    }
    tinymt32j_status_write(d_status, &tiny);
}
