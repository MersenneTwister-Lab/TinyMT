#ifndef TINYMT64DEF_H
#define TINYMT64DEF_H
/**
 * @file tinymt64def.h
 *
 * @brief Common definitions in host and kernel for 64-bit tinymt.
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

#define TINYMT64J_MAT1 0xfa051f40U
#define TINYMT64J_MAT2 0xffd0fff4U;
#define TINYMT64J_TMAT UINT64_C(0x58d02ffeffbfffbc)

#endif
