/**
 * @file test64_jump2.cl
 *
 * @brief Test Program for openCL 1.2
 *
 * This program generates 64-bit unsigned integers.
 * The period of generated integers is 2<sup>127</sup>-1.
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
#include "tinymt64_jump.clh"

/**
 * kernel function.
 * This function initialize internal state of tinymt64.
 *
 * @param d_status internal state of kernel side tinymt
 * @param jump_table jump_table for initial jump
 * @param seed seed for initialization
 */
__kernel void
tinymt_init_seed_kernel(__global tinymt64j_t * d_status,
			__constant uint * jump_table,
			ulong seed)
{
    tinymt64j_t tiny;
    size_t gid = tinymt_get_sequential_id();

    tinymt64j_init_seed(&tiny, seed);
    for (int i = 0; gid != 0; i++) {
        if ((gid & 1) != 0) {
            tinymt64j_jump_by_array(&tiny, &jump_table[i * 4]);
        }
        gid = gid >> 1;
    }
    tinymt64j_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function changes internal state of tinymt64 for next loop.
 *
 * @param d_status internal state of kernel side tinymt
 * @param jump_table jump table for next loop
 */
__kernel void
tinymt_jump_kernel(__global tinymt64j_t * d_status,
		   __constant uint * jump_table)
{
    tinymt64j_t tiny;
    size_t gid = tinymt_get_sequential_id();

    tinymt64j_status_read(&tiny, d_status);
    tinymt64j_jump_by_array(&tiny, jump_table);
    tinymt64j_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function generates 64-bit unsigned integers in d_data
 *
 * @param d_status internal state of kernel side tinymt
 * @param d_data output
 * @param size number of output data for a work item.
 */
__kernel void
tinymt_uint64_kernel(__global tinymt64j_t * d_status,
		     __global ulong * d_data,
		     int size)
{
    const size_t id = tinymt_get_sequential_id();
    const size_t global_size = tinymt_get_sequential_size();
    tinymt64j_t tiny;

    tinymt64j_status_read(&tiny, d_status);
    for (int i = 0; i < size; i++) {
	d_data[size * id + i] = tinymt64j_uint64(&tiny);
    }
    tinymt64j_status_write(d_status, &tiny);
}
