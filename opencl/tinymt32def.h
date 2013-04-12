#ifndef TINYMT32DEF_H
#define TINYMT32DEF_H
/**
 * @file tinymt32def.h
 *
 * @brief Common definitions in host and kernel for 32-bit tinymt.
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

#if defined(KERNEL_PROGRAM)
#if !defined(cl_uint)
#define cl_uint uint
#endif
#endif

/**
 * TinyMT32 structure with parameters
 */
typedef struct TINYMT32WP_T {
    cl_uint s0;
    cl_uint s1;
    cl_uint s2;
    cl_uint s3;
    cl_uint mat1;
    cl_uint mat2;
    cl_uint tmat;
} tinymt32wp_t;

/**
 * TinyMT32 structure for jump without parameters
 */
typedef struct TINYMT32J_T {
    cl_uint s0;
    cl_uint s1;
    cl_uint s2;
    cl_uint s3;
} tinymt32j_t;

#define TINYMT32J_MAT1 0x8f7011eeU
#define TINYMT32J_MAT2 0xfc78ff1fU
#define TINYMT32J_TMAT 0x3793fdffU

#endif
