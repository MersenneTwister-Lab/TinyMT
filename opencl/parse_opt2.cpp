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
    do {
	if (argc <= 3) {
	    error = true;
	    break;
	}
	opt.group_num = strtol(argv[1], NULL, 10);
	if (errno) {
	    error = true;
	    cerr << "group num error!" << endl;
	    cerr << strerror(errno) << endl;
	    break;
	}
	opt.local_num = strtol(argv[2], NULL, 10);
	if (errno) {
	    error = true;
	    cerr << "local num error!" << endl;
	    cerr << strerror(errno) << endl;
	    break;
	}
	opt.data_count = strtol(argv[3], NULL, 10);
	if (errno) {
	    error = true;
	    cerr << "data count error!" << endl;
	    cerr << strerror(errno) << endl;
	    break;
	}
    } while (0);

    if (error) {
	cerr << pgm
	     << " group-num local-num data-count" << endl;
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
