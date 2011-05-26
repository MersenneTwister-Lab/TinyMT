#ifndef PARSE_OPT_H
#define PARSE_OPT_H
/**
 * @file parse_opt.h
 *
 * @brief Command line option parser for tinymt32dc and tinymt64dc.
 *
 * Command line option parser for tinymt32dc and tinymt64dc.
 * Users can change this file so that it fits to their applications
 * and OS, without serious influence on other programs of tinymtdc.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */

#include <stdint.h>
#include <string>

class tinymt_options {
public:
    bool verbose;
    bool all;
    std::string filename;
    uint32_t uid;
    long long count;
    uint32_t start;
    int max_delta;
};

bool parse_opt(tinymt_options& opt, int argc, char **argv, uint32_t start);

#endif
