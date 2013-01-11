#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "parse_opt.h"

using namespace std;

bool parse_opt(options& opt, int argc, char **argv) {
#if defined(DEBUG)
    cout << "parse_opt start" << endl;
#endif
    int c;
    bool error = false;
    string pgm = argv[0];
    errno = 0;
    if (argc <= 4) {
	error = true;
    }
    while (!error) {
	opt.filename = argv[1];
	opt.group_num = strtol(argv[2], NULL, 10);
	if (errno) {
	    error = true;
	    cerr << "group num error!" << endl;
	    cerr << strerror(errno) << endl;
	    break;
	}
	opt.local_num = strtol(argv[3], NULL, 10);
	if (errno) {
	    error = true;
	    cerr << "local num error!" << endl;
	    cerr << strerror(errno) << endl;
	    break;
	}
	opt.data_count = strtol(argv[4], NULL, 10);
	if (errno) {
	    error = true;
	    cerr << "data count error!" << endl;
	    cerr << strerror(errno) << endl;
	    break;
	}
	if (!opt.filename.empty()) {
	    ifstream ifs(opt.filename.c_str());
	    if (ifs) {
		ifs.close();
	    } else {
		error = true;
		cerr << "can't open file:" << opt.filename << endl;
		break;
	    }
	}
	break;
    }
    if (error) {
	cerr << pgm
	     << " paramfile group-num local-num data-count" << endl;
	cerr << "paramfile   parameter file of tinymt." << endl;
	cerr << "group-num   group number of kernel call." << endl;
	cerr << "local-num   local item number of kernel cal." << endl;
	cerr << "data-count  generate random number count." << endl;
	return false;
    }
#if defined(DEBUG)
    cout << "parse_opt end" << endl;
#endif
    return true;
}
