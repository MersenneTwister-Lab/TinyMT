/** -*- c -*-
 * @file sample.cl
 *
 * @brief Sample Program for openCL 1.1
 */
#include "tinymt32st.clh"
/**
 * kernel function.
 * This function generates 32-bit unsigned integers in d_data
 *
 * @param[in,out] d_status kernel I/O data
 * @param[out] d_data output
 * @param[in] size number of output data requested.
 */
__kernel void tinymt32_uint32_kernel(__glogal uint* params,
				   __global uint* d_sum,
				   int size)
{
    const int tid = get_global_id(0);
    const int global_size = get_global_size(0);
    __global const uint *p = params + tid * 3;
    tinymt32_t tiny;
    uint sum = 0;

    // set parameter
    tiny.mat1 = p[0];
    tiny.mat2 = p[1];
    tiny.tmat = p[2];
    // initialize
    tinymt32_init(&tiny, tid);

    for (int i = 0; i < size; i++) {
	// generate
	sum += tinymt32_uint32(&tiny);
    }
    d_sum[tid] = sum;
}

/**
 * kernel function.
 * This function generates 32-bit unsigned integers in d_data
 *
 * @param[in,out] d_status kernel I/O data
 * @param[out] d_data output
 * @param[in] size number of output data requested.
 */
__kernel void tinymt32_single_kernel(__global uint* params,
				   __global float* d_sum,
				   int size)
{
    const int tid = get_global_id(0);
    const int global_size = get_global_size(0);
    __global const uint *p = params + tid * 3;
    tinymt32_t tiny;
    float sum = 0;

    // set parameter
    tiny.mat1 = p[0];
    tiny.mat2 = p[1];
    tiny.tmat = p[2];
    // initialize
    tinymt32_init(&tiny, tid);

    for (int i = 0; i < size; i++) {
	// generate
	sum += tinymt32_single(&tiny);
    }
    d_sum[tid] = sum;
}

