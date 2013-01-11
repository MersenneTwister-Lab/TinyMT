/**
 * @file tinymt_host.hpp
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
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
	void get(uint32_t * mat1, uint32_t * mat2, uint32_t * tmat) {
	    using namespace std;
	    char buffer[bufsize];
	    if (ifs) {
		ifs.seekg(pos);
		for(;;) {
		    ifs.getline((char *)buffer, 500);
		    if (buffer[0] != '#') {
			break;
		    }
		}
		get_params(mat1, mat2, tmat, buffer);
	    } else {
		cerr << "filename:" << filename << endl;
		throw runtime_error("file not found");
	    }
	};
	file_reader(const std::string& filename) {
	    ifs.open(filename.c_str(), ios::in);
	};
	~file_reader() {
	    ifs.close();
	};
    private:
	enum {bufsize = 500};
	ifstream ifs;
	char * search_comma_next(char * buffer, int count) {
	    using namespace std;
	    for (int i = 0; i < bufsize; i++) {
		if (buffer[i] == '\0') {
		    cerr << "comma not found:" << buffer << endl;
		    throw runtime_error("comma not found");
		}
		if (buffer[i] == ',') {
		    count--;
		}
		if (count == 0) {
		    return &buffer[i + 1];
		}
	    }
	    cerr << "comma not found:" << buffer << endl;
	    throw runtime_error("comma not found");
	};
	void get_params(uint32_t *mat1, uint32_t *mat2, uint32_t *tmat,
			char *buffer) {
	    using namespace std;
	    char * p;
	    errno = 0;
	    p = search_comma_next(buffer, 3);
	    *mat1 = strtoul(p, NULL, 16);
	    p = search_comma_next(buffer, 4);
	    *mat2 = strtoul(p, NULL, 16);
	    p = search_comma_next(buffer, 5);
	    *tmat = strtoul(p, NULL, 16);
	    if (errno != 0) {
		cerr << "file format error:" << buffer << endl;
		throw runtime_error("file format error");
	    }
	};
    };
}

#endif
