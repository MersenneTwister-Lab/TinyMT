/**
 * @file test32.cl
 *
 * @brief Test Program for openCL 1.2
 *
 * tinymt32
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
 * @param d_status internal state of kernel side tinymt
 * @param seed seed of initialization
 */
__kernel void
tinymt_init_seed_kernel(__global tinymt32wp_t * d_status,
                        uint seed)
{
    const size_t id = tinymt_get_sequential_id();
    tinymt32wp_t tiny;

    tinymt32_status_read(&tiny, d_status);
    tinymt32_init(&tiny, seed + id);
    tinymt32_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function initialize internal state of tinymt32.
 *
 * @param d_status internal state of kernel side tinymt
 * @param seeds array of seeds for initialization.
 * @param length length of seeds.
 */
__kernel void
tinymt_init_array_kernel(__global tinymt32wp_t * d_status,
                         __global uint * seeds,
                        int length)
{
    const size_t id = tinymt_get_sequential_id();
    tinymt32wp_t tiny;
    uint local_seeds[10];
    if (length > 10) {
        length = 10;
    }
    for (int i = 0; i < length; i++) {
        local_seeds[i] = seeds[i];
    }
    tinymt32_status_read(&tiny, d_status);
    tinymt32_init_by_array(&tiny, local_seeds, length);
    tinymt32_status_write(d_status, &tiny);
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
tinymt_uint32_kernel(__global tinymt32wp_t * d_status,
                     __global uint * d_data,
                     int size)
{
    const size_t id = tinymt_get_sequential_id();
    const size_t global_size = tinymt_get_sequential_size();
    tinymt32wp_t tiny;

    tinymt32_status_read(&tiny, d_status);
    for (int i = 0; i < size; i++) {
        d_data[global_size * i + id] = tinymt32_uint32(&tiny);
    }
    tinymt32_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function generates floating points in the range [1, 2) in d_data
 *
 * @param d_status internal state of kernel side tinymt
 * @param d_data output
 * @param size number of output data requested.
 */
__kernel void
tinymt_single12_kernel(__global tinymt32wp_t * d_status,
                       __global float * d_data,
                       int size)
{
    const size_t id = tinymt_get_sequential_id();
    const size_t sequential_size = tinymt_get_sequential_size();
    tinymt32wp_t tiny;

    tinymt32_status_read(&tiny, d_status);
    for (int i = 0; i < size; i++) {
        d_data[sequential_size * i + id] = tinymt32_single12(&tiny);
    }
    tinymt32_status_write(d_status, &tiny);
}

/**
 * kernel function.
 * This function generates floating points in the range [0,1) in d_data
 *
 * @param[in,out] d_status internal state of kernel side tinymt
 * @param[out] d_data output
 * @param[in] size number of output data requested.
 */
__kernel void
tinymt_single01_kernel(__global tinymt32wp_t * d_status,
                       __global float * d_data,
                       int size)
{
    const size_t id = tinymt_get_sequential_id();
    const size_t sequential_size = tinymt_get_sequential_size();
    tinymt32wp_t tiny;

    tinymt32_status_read(&tiny, d_status);
    for (int i = 0; i < size; i++) {
        d_data[sequential_size * i + id] = tinymt32_single01(&tiny);
    }
    tinymt32_status_write(d_status, &tiny);
}

