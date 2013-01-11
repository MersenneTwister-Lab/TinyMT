/**
 * @file sample32.cl
 *
 * @brief Sample Program for openCL 1.2
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
#include "tinymt32.clh"

/**
 * kernel function.
 * This function initialize internal state of tinymt32.
 *
 * @param[in,out] d_status kernel I/O data
 * @param[out] d_data output
 * @param[in] size number of output data requested.
 */
__kernel void
calc_pi(__global tinymt32wp_t * d_status,
	       uint seed,
	       int num,
	       __global float * pi,
	       __global uint * global_sum,
	       __local uint * local_sum)
{
    const size_t id = tinymt_get_sequential_id();
    //const size_t total = tinymt_get_sequential_size();
    const size_t total = tinymt_get_sequential_size();
    tinymt32wp_t tiny;
    const size_t group_id = get_group_id(0);
    const size_t local_id = get_local_id(0);
    uint sum_all = 0;

    tinymt32_status_read(&tiny, d_status);
    tinymt32_init(&tiny, seed + id);
    local_sum[local_id] = 0;
    for (int i = 0; i < num; i++) {
	float x = tinymt32_single01(&tiny);
	float y = tinymt32_single01(&tiny);
	if (x * x + y * y < 1.0f) {
	    local_sum[local_id]++;
	}
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    if (local_id == 0) {
	global_sum[group_id] = 0;
	for (uint i = 0; i < get_local_size(0); i++) {
	    global_sum[group_id] += local_sum[i];
	}
    }
    barrier(CLK_GLOBAL_MEM_FENCE);
    if (get_local_id(0) == 0 && get_group_id(0) == 0) {
	for (int i = 0; i < get_num_groups(0); i++) {
	    sum_all += global_sum[i];
	}
	*pi = (4.0f * sum_all) / total / num;
    }
}
