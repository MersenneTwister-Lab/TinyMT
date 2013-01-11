#ifndef TINYMT32CL_H
#define TINYMT32CL_H
/**
 * @file tinymt32cl.h
 *
 * @brief Sample Program for openCL 1.1
 *
 * tinymt32 (pattern H)
 * This program generates 32-bit unsigned integers.
 * The period of generated integers is 2<sup>127</sup>-1.
 *
 * This also generates single precision floating point numbers
 * uniformly distributed in the range [1, 2). (float r; 1.0 <= r < 2.0)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2010 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */

/**
 * kernel I/O
 * This structure must be initialized before first use.
 */
typedef struct TINYMT32CL {
    uint status[4];
    uint mat1;
    uint mat2;
    uint tmat;
} tinymt32cl;

#endif
