/**
 * @file file_reader.cpp
 *
 * @brief Utility program for reading parameter files.
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
#include <iostream>
#include <iomanip>
#include "file_reader.h"
namespace tinymt {
    using namespace std;

    /**
     * get parameters from prameter file of 32-bit tinymt
     *@param mat1 mat1 parameter
     *@param mat2 mat2 parameter
     *@param tmat tmat parameter
     */
    void file_reader::get(uint32_t * mat1, uint32_t * mat2, uint32_t * tmat) {
	char buffer[bufsize];
	ifstream ifs(filename.c_str(), ios::in);
	if (ifs) {
	    ifs.seekg(pos);
	    for(;;) {
		ifs.getline((char *)buffer, 500);
		if (buffer[0] != '#') {
		    break;
		}
	    }
	    get_params(mat1, mat2, tmat, buffer);
	    pos = ifs.tellg();
	    ifs.close();
	} else {
	    cerr << "filename:" << filename << endl;
	    throw runtime_error("file not found");
	}
    }

    /**
     * get parameters from prameter file of 64-bit tinymt
     *@param mat1 mat1 parameter
     *@param mat2 mat2 parameter
     *@param tmat tmat parameter
     */
    void file_reader::get(uint32_t * mat1, uint32_t * mat2, uint64_t * tmat) {
	char buffer[bufsize];
	ifstream ifs(filename.c_str(), ios::in);
	if (ifs) {
	    ifs.seekg(pos);
	    for(;;) {
		ifs.getline((char *)buffer, 500);
		if (buffer[0] != '#') {
		    break;
		}
	    }
	    get_params(mat1, mat2, tmat, buffer);
	    pos = ifs.tellg();
	    ifs.close();
	} else {
	    cerr << "filename:" << filename << endl;
	    throw runtime_error("file not found");
	}
    }

    /**
     * constructor from filename
     *@param p_filename file name of prameter file
     */
    file_reader::file_reader(const std::string& p_filename) {
	filename = p_filename;
	pos = 0;
    }

    /**
     * search nth next comma in string
     *@param buffer string
     *@param count specify nth comma
     *@return pointer to next char of nth comma
     */
    char * file_reader::search_comma_next(char * buffer, int count) {
	for (int i = 0; i < bufsize; i++) {
	    if (buffer[i] == '\0') {
		cerr << "pos:" << dec << pos
		     << " comma is not found:" << buffer << endl;
		throw runtime_error("comma not found");
	    }
	    if (buffer[i] == ',') {
		count--;
	    }
	    if (count == 0) {
		return &buffer[i + 1];
	    }
	}
	cerr << " pos:" << dec << pos
	     << "comma is not found:" << buffer << endl;
	throw runtime_error("comma was not found");
    }

    /**
     * get parameters from buffer
     *@param mat1 mat1 parameter
     *@param mat2 mat2 parameter
     *@param tmat tmat parameter
     */
    void file_reader::get_params(uint32_t *mat1, uint32_t *mat2, uint32_t *tmat,
				 char *buffer) {
	char * p;
	errno = 0;
#if defined(DEBUG) && 0
	cout << "get_params start" << endl;
#endif
	for (;;) {
	    if (buffer[0] == '\n' || buffer[0] == '\0') {
		errno = 1;
		break;
	    }
	    p = search_comma_next(buffer, 3);
	    *mat1 = strtoul(p, NULL, 16);
	    p = search_comma_next(buffer, 4);
	    *mat2 = strtoul(p, NULL, 16);
	    p = search_comma_next(buffer, 5);
	    *tmat = strtoul(p, NULL, 16);
	    break;
	}
	if (errno != 0) {
	    cerr << "file format error:" << buffer << endl;
	    throw runtime_error("file format error");
	}
#if defined(DEBUG) && 0
	cout << "get_params end" << endl;
#endif
    }

    /**
     * get parameters from buffer
     *@param mat1 mat1 parameter
     *@param mat2 mat2 parameter
     *@param tmat tmat parameter
     */
    void file_reader::get_params(uint32_t *mat1, uint32_t *mat2, uint64_t *tmat,
				 char *buffer) {
	char * p;
	errno = 0;
#if defined(DEBUG) && 0
	cout << "get_params start" << endl;
#endif
	for (;;) {
	    if (buffer[0] == '\n' || buffer[0] == '\0') {
		errno = 1;
		break;
	    }
	    p = search_comma_next(buffer, 3);
	    *mat1 = strtoul(p, NULL, 16);
	    p = search_comma_next(buffer, 4);
	    *mat2 = strtoul(p, NULL, 16);
	    p = search_comma_next(buffer, 5);
	    *tmat = strtoull(p, NULL, 16);
	    break;
	}
	if (errno != 0) {
	    cerr << "file format error:" << buffer << endl;
	    throw runtime_error("file format error");
	}
#if defined(DEBUG) && 0
	cout << "get_params end" << endl;
#endif
    }
}

#if defined(MAIN)
int main(int argc, char * argv[]) {
    using namespace tinymt;
    using namespace std;

    file_reader r(argv[1]);
    uint32_t mat1;
    uint32_t mat2;
    uint32_t tmat;
    for (int i = 0; i < 10; i++) {
	r.get(&mat1, &mat2, &tmat);
	cout << "mat1:" << hex << mat1 << endl;
	cout << "mat2:" << hex << mat2 << endl;
	cout << "tmat:" << hex << tmat << endl;
    }
}
#endif
