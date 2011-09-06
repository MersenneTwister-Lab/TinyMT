/**
 * @file parse_opt.cpp
 *
 * @brief Command line option parser for tinymt32dc and tinymt64dc.
 *
 * Command line option parser for tinymt32dc and tinymt64dc.
 * Users can change this file so that it fits to their applications
 * and OS.
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
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "parse_opt.h"

using namespace std;

static void output_help(string& pgm, uint32_t start);

/**
 * command line option parser
 * @param opt a structure to keep the result of parsing
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @param start default start value
 * @return command line options have error, or not
 */
bool parse_opt(tinymt_options& opt, int argc, char **argv, uint32_t start) {
    int c;
    bool error = false;
    string pgm = argv[0];
    static struct option longopts[] = {
	{"verbose", no_argument, NULL, 'v'},
	{"all", no_argument, NULL, 'a'},
	{"file", required_argument, NULL, 'f'},
	{"start", required_argument, NULL, 's'},
	{"max", required_argument, NULL, 'm'},
	{"count", required_argument, NULL, 'c'},
	{NULL, 0, NULL, 0}};
    opt.verbose = false;
    opt.count = 1;
    opt.max_delta = 10;
    opt.start = start;
    opt.all = false;
    errno = 0;
    for (;;) {
	c = getopt_long(argc, argv, "vas:m:f:c:", longopts, NULL);
	if (error) {
	    break;
	}
	if (c == -1) {
	    break;
	}
	switch (c) {
	case 'a':
	    opt.all = true;
	    break;
	case 'v':
	    opt.verbose = true;
	    break;
	case 's':
	    opt.start = strtoull(optarg, NULL, 0);
	    if (errno) {
		error = true;
		cerr << "start must be a number" << endl;
	    }
	    if (opt.start > start) {
		error = true;
		cerr << "start must be less than or equal to " << start << endl;
	    }
	    break;
	case 'f':
	    opt.filename = optarg;
	    break;
	case 'm':
	    opt.max_delta = strtol(optarg, NULL, 10);
	    if (errno) {
		error = true;
		cerr << "max must be a number" << endl;
	    }
	    break;
	case 'c':
	    opt.count = strtoll(optarg, NULL, 10);
	    if (errno) {
		error = true;
		cerr << "count must be a number" << endl;
	    }
	    break;
	case '?':
	default:
	    error = true;
	    break;
	}
    }
    argc -= optind;
    argv += optind;
    if (argc < 1) {
	error = true;
    } else {
	long long id = strtoll(argv[0], NULL, 10);
	if (errno != 0 || (id < 0 || id > 0xffffffff)) {
	    error = true;
	    cerr << "id must be a number between 0 and " << 0xffffffff
		 << endl;
	}
	opt.uid = static_cast<uint32_t>(id);
    }
    if (!opt.filename.empty()) {
	ofstream ofs(opt.filename.c_str());
	if (ofs) {
	    ofs.close();
	} else {
	    error = true;
	    cerr << "can't open file:" << opt.filename << endl;
	}
    }
    if (error) {
	output_help(pgm, start);
	return false;
    }
    return true;
}

/**
 * showing help message
 * @param pgm program name
 */
static void output_help(string& pgm, uint32_t start) {
    cerr << "usage:" << endl;
    cerr << pgm
	 << " [-v] [-c count] [-a] [-s start_pos] [-m max-delta]"
	 << " [-f outputfile]"
	 << " id" << endl;
    static string help_string1 = "\n"
"id                   ID of generator. the parameters searched with different\n"
"                     ids will generate different sequences.\n"
"--start, -s pos      Parameters are searched from this number by counting\n"
	"                     down. If omitted ";
    static string help_string2 = " is assumed.\n"
"--verbose, -v        Verbose mode. Output parameters, calculation time, etc.\n"
"--file, -f filename  Parameters are outputted to this file. without this\n"
"                     option, parameters are outputted to standard output.\n"
"--count, -c count    Output count. The number of parameters to be outputted.\n"
"--all, -a            Search and output all parameters for the specified id.\n"
"--max, -m max-delta  Search parameters whose total dimension defect is\n"
"                     smaller than or equal to max-delta. if 0 is specified,\n"
"                     search parameters for maximally equidistributed\n"
"                     generators.";
    cerr << help_string1 << "0x" << hex << start << help_string2 << endl;
}
