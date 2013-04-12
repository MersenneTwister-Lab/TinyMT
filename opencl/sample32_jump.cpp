/**
 * Sample program for OpenCL
 * using a parameter and jump function
 */
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define __CL_ENABLE_EXCEPTIONS

#include <cstddef>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <float.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "opencl_tools.hpp"
#include "tinymt32def.h"
#include "tinymt32.h"

using namespace std;
using namespace cl;

/* ================== */
/* OpenCL information */
/* ================== */
std::vector<cl::Platform> platforms;
std::vector<cl::Device> devices;
cl::Context context;
std::string programBuffer;
cl::Program program;
cl::Program::Sources source;
cl::CommandQueue queue;
std::string errorMessage;

/* =========================
   declaration
   ========================= */
static int group_num;
static int local_num;
static int data_count;

static bool parse_opt(int argc, char **argv);
static void calc_pi(int total_num,
                    int local_num,
                    int data_size);
static int sample(int argc, char * argv[]);

/**
 * main
 * call sample main and catch errors
 *@param argc number of arguments
 *@param argv array of arguments
 *@return 0 normal, -1 error
 */
int main(int argc, char * argv[])
{
    try {
        return sample(argc, argv);
    } catch (Error e) {
        cerr << "Error Code:" << e.err() << endl;
        cerr << e.what() << endl;
    }
    return -1;
}

/**
 * sample main
 *@param argc number of arguments
 *@param argv array of arguments
 *@return 0 normal, -1 error
 */
static int sample(int argc, char * argv[])
{
#if defined(DEBUG)
    cout << "sample start" << endl;
#endif
    if (!parse_opt(argc, argv)) {
        return -1;
    }
    // OpenCL setup
#if defined(DEBUG)
    cout << "openCL setup start" << endl;
#endif
    platforms = getPlatforms();
    devices = getDevices();
    context = getContext();
#if defined(INCLUDE_IMPOSSIBLE)
    source = getSource("sample32_jump.cli");
#else
    source = getSource("sample32_jump.cl");
#endif
    std::string option = "-DKERNEL_PROGRAM ";
#if defined(DEBUG)
    option += "-DDEBUG ";
#endif
    program = getProgram(option.c_str());
    queue = getCommandQueue();
#if defined(DEBUG)
    cout << "openCL setup end" << endl;
#endif
    int total_num = group_num * local_num;
    int max_group_size = getMaxGroupSize();
    if (group_num > max_group_size) {
        cout << "group_num greater than max value("
             << max_group_size << ")"
             << endl;
        return -1;
    }
    calc_pi(total_num, local_num, data_count);
    return 0;
}

/**
 * calculate PI using Monte Carlo Simulation
 *@param total_num total number of work items
 *@param local_num number of local work items
 *@param data_size number of data to generate
 */
static void calc_pi(int total_num,
                    int local_num,
                    int data_size)
{
#if defined(DEBUG)
    cout << "calc_pi start" << endl;
#endif
    int group_num = total_num / local_num;
    int min_size = total_num;
    if (data_size % min_size != 0) {
        data_size = (data_size / min_size + 1) * min_size;
    }
    uint32_t seed = 1234;
    Kernel uint_kernel(program, "calc_pi");
    Buffer global_buffer(context,
                         CL_MEM_READ_WRITE,
                         sizeof(uint32_t) * group_num);

    uint_kernel.setArg(0, seed);
    uint_kernel.setArg(1, data_size / total_num);
    uint_kernel.setArg(2, global_buffer);
    uint_kernel.setArg(3, sizeof(uint32_t) * local_num, NULL);
    NDRange global(total_num);
    NDRange local(local_num);
    Event generate_event;
#if defined(DEBUG)
    cout << "calc_pi enque kernel start" << endl;
#endif
    queue.enqueueNDRangeKernel(uint_kernel,
                               NullRange,
                               global,
                               local,
                               NULL,
                               &generate_event);
    double pi = 0;
    uint32_t *global_sum = new uint32_t[group_num];
    generate_event.wait();
    queue.enqueueReadBuffer(global_buffer,
                            CL_TRUE,
                            0,
                            sizeof(uint32_t) * group_num,
                            global_sum);
    double time = get_time(generate_event);
    for (int i = 0; i < group_num; i++) {
        pi += global_sum[i];
    }
    pi = 4.0 * pi / data_size;
    cout << "generate time:" << time * 1000 << "ms" << endl;
    cout << "calculated pi = " << pi << endl;
    delete[] global_sum;
#if defined(DEBUG)
    cout << "calc_pi end" << endl;
#endif
}

/**
 * parsing command line options
 *@param argc number of arguments
 *@param argv array of argument strings
 *@return true if errors are found in command line arguments
 */
static bool parse_opt(int argc, char **argv)
{
#if defined(DEBUG)
    cout << "parse_opt start" << endl;
#endif
    bool error = false;
    std::string pgm = argv[0];
    errno = 0;
    if (argc <= 3) {
        error = true;
    }
    while (!error) {
        group_num = strtol(argv[1], NULL, 10);
        if (errno) {
            error = true;
            cerr << "group num error!" << endl;
            cerr << strerror(errno) << endl;
            break;
        }
        if (group_num <= 0) {
            error = true;
            cerr << "group num should be greater than zero." << endl;
            break;
        }
        local_num = strtol(argv[2], NULL, 10);
        if (errno) {
            error = true;
            cerr << "local num error!" << endl;
            cerr << strerror(errno) << endl;
            break;
        }
        if (local_num <= 0) {
            error = true;
            cerr << "local num should be greater than zero." << endl;
            break;
        }
        data_count = strtol(argv[3], NULL, 10);
        if (errno) {
            error = true;
            cerr << "data count error!" << endl;
            cerr << strerror(errno) << endl;
            break;
        }
        break;
    }
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
