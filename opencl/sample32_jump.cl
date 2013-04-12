/**
 * @file sample32_jump.cl
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
#include "tinymt32_jump.clh"

/**
 * kernel function.
 * This function calculates PI by Monte Carlo Method.
 * Parameters for tinymt are not needed in arguments list, because
 * we use only one parameter set and that is defined in tinymt32_jump.clh.
 *
 * @param seed seed for initialization
 * @param num number of random numbers to generate
 * @param global_sum number of points in a quadrant
 * @param local_sum temporary summation of points in a quadrant
 */
__kernel void
calc_pi(uint seed,
        int num,
        __global uint * global_sum,
        __local uint * local_sum)
{
    const size_t total = tinymt_get_sequential_size();
    tinymt32j_t tiny;
    const size_t group_id = get_group_id(0);
    const size_t local_id = get_local_id(0);
    uint sum = 0;

    tinymt32j_init_jump(&tiny, seed);
    for (int i = 0; i < num; i++) {
        float x = tinymt32j_single01(&tiny);
        float y = tinymt32j_single01(&tiny);
        if (x * x + y * y < 1.0f) {
            sum++;
        }
    }
    local_sum[local_id] = sum;
    barrier(CLK_LOCAL_MEM_FENCE);
    sum = 0;
    if (local_id == 0) {
        for (uint i = 0; i < get_local_size(0); i++) {
            sum += local_sum[i];
        }
        global_sum[group_id] = sum;
    }
}
