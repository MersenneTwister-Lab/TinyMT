/**
 * @file sample32.cl
 *
 * @brief Sample Program for openCL 1.2
 *
 * This sample program calculates PI by Monte Carlo Method
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
 * This function calculates PI by Monte Carlo Method
 *
 * @param d_status internal state of tinymt
 * @param seed seed for initialization
 * @param size number of random numbers to generate
 * @param global_sum number of points in a quadrant
 * @param local_sum temporary summation of points in a quadrant
 */
__kernel void
calc_pi(__global tinymt32wp_t * d_status,
               uint seed,
               int size,
               __global uint * global_sum,
               __local uint * local_sum)
{
    tinymt32wp_t tiny;
    const size_t group_id = get_group_id(0);
    const size_t local_id = get_local_id(0);
    const size_t local_size = get_local_size(0);
    const size_t id = tinymt_get_sequential_id();

    // initialize
    tinymt32_status_read(&tiny, d_status);
    tinymt32_init(&tiny, seed + id);
    uint sum = 0;
    for (int i = 0; i < size; i++) {
        // generate
        float x = tinymt32_single01(&tiny);
        float y = tinymt32_single01(&tiny);
        if (x * x + y * y < 1.0f) {
            sum++;
        }
    }
    local_sum[local_id] = sum;
    // partial reduce
    barrier(CLK_LOCAL_MEM_FENCE);
    sum = 0;
    if (local_id == 0) {
        for (uint i = 0; i < local_size; i++) {
            sum += local_sum[i];
        }
        global_sum[group_id] = sum;
    }
}
