#ifndef TINYMT64DEF_H
#define TINYMT64DEF_H
/**
 * @file tinymt64def.h
 *
 * @brief Sample Program for openCL 1.2
 *
 * tinymt64
 * This program generates 32-bit unsigned integers.
 * The period of generated integers is 2<sup>127</sup>-1.
 *
 * This also generates double precision floating point numbers
 * uniformly distributed in the range [1, 2). (double r; 1.0 <= r < 2.0)
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
typedef struct TINYMT64WP_T {
    ulong s0;
    ulong s1;
    uint mat1;
    uint mat2;
    ulong tmat;
} tinymt64wp_t;

/**
 * TinyMT32 structure for jump without parameters
 */
typedef struct TINYMT64J_T {
    ulong s0;
    ulong s1;
} tinymt64j_t;

#endif
