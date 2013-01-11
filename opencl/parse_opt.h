#ifndef PARSE_OPT_H
#define PARSE_OPT_H

#include <stdint.h>
#include <string>

class options {
public:
    std::string filename;
    int group_num;
    int local_num;
    int data_count;
};

bool parse_opt(options& opt, int argc, char **argv);

#endif
