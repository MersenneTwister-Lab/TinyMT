#ifndef TINYMT32DEF_H
#define TINYMT32DEF_H
/**
 * @file tinymt32def.h
 *
 * @brief Sample Program for openCL 1.2
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

/**
 * TinyMT32 structure with parameters
 */
typedef struct TINYMT32WP_T {
    uint s0;
    uint s1;
    uint s2;
    uint s3;
    uint mat1;
    uint mat2;
    uint tmat;
} tinymt32wp_t;

/**
 * TinyMT32 structure for jump without parameters
 */
typedef struct TINYMT32J_T {
    uint s0;
    uint s1;
    uint s2;
    uint s3;
} tinymt32j_t;

#endif
