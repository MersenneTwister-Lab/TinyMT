#ifndef FILE_READER_H
#define FILE_READER_H
/**
 * @file file_reaer.h
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state
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
#include <stdint.h>
#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <stdexcept>
#include <exception>

namespace tinymt {

    class file_reader {
    public:
	void get(uint32_t * mat1, uint32_t * mat2, uint32_t * tmat);
	void get(uint32_t * mat1, uint32_t * mat2, uint64_t * tmat);
	file_reader(const std::string& p_filename);
    private:
	enum {bufsize = 500};
	std::string filename;
	int pos;
	char * search_comma_next(char * buffer, int count);
	void get_params(uint32_t *mat1, uint32_t *mat2, uint32_t *tmat,
			char *buffer);
	void get_params(uint32_t *mat1, uint32_t *mat2, uint64_t *tmat,
			char *buffer);
    };
}
#endif

