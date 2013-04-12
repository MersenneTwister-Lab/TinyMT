/**
 * Sample program for OpenCL
 * using 1 parameter for 1 generator
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
#include <stdint.h>
#include <inttypes.h>

#include "opencl_tools.hpp"
#include "tinymt64def.h"
#include "tinymt64.h"
#include "file_reader.h"

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
static std::string filename;
static int group_num;
static int local_num;
static int data_count;
static bool parse_opt(int argc, char **argv);
static Buffer get_param_buff(std::string& filename,
                             int total_num);
static void calc_pi(Buffer& tinymt_status,
                    int total_num,
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

    if (!hasDoubleExtension()) {
        cout << "the GPU device does not have double extension." << endl;
        return -1;
    }
#if defined(INCLUDE_IMPOSSIBLE)
    source = getSource("sample64.cli");
#else
    source = getSource("sample64.cl");
#endif
    std::string option = "-DKERNEL_PROGRAM -DHAVE_DOUBLE ";
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
    Buffer tinymt_status = get_param_buff(filename, total_num);
    calc_pi(tinymt_status, total_num, local_num, data_count);
    return 0;
}

/**
 * calculate PI using Monte Carlo Simulation
 *@param tinymt_status internal state of kernel side tinymt
 *@param total_num total number of work items
 *@param local_num number of local work items
 *@param data_size number of data to generate
 */
static void calc_pi(Buffer& tinymt_status,
                            int total_num,
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
    uint64_t seed = 1234;
    Kernel uint_kernel(program, "calc_pi");
    Buffer global_buffer(context,
                         CL_MEM_READ_WRITE,
                         sizeof(uint32_t) * group_num);

    uint_kernel.setArg(0, tinymt_status);
    uint_kernel.setArg(1, seed);
    uint_kernel.setArg(2, data_size / total_num);
    uint_kernel.setArg(3, global_buffer);
    uint_kernel.setArg(4, sizeof(uint32_t) * local_num, NULL);
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
    uint32_t * result = new uint32_t[group_num];
    double pi = 0;
    generate_event.wait();
    queue.enqueueReadBuffer(global_buffer,
                            CL_TRUE,
                            0,
                            sizeof(uint32_t) * group_num,
                            result);
    double time = get_time(generate_event);
    for (int i = 0; i < group_num; i++) {
        pi += result[i];
    }
    pi = pi * 4 / data_size;
    cout << "generate time:" << time * 1000 << "ms" << endl;
    cout << "calculated pi = " << pi << endl;
    delete[] result;
#if defined(DEBUG)
    cout << "calc_pi end" << endl;
#endif
}

/* ==============
 * utility programs
 * ==============*/
/**
 * get buffer for kernel side tinymt
 *@param filename name of parameter file generated by tinymt64dc
 *@param total_num total number of work items
 *@return buffer for kernel side tinymt
 */
static Buffer get_param_buff(std::string& filename,
                             int total_num)
{
#if defined(DEBUG)
    cout << "get_rec_buff start" << endl;
#endif
    tinymt::file_reader fr(filename);
    tinymt64wp_t * status_tbl = new tinymt64wp_t[total_num];
    uint32_t mat1;
    uint32_t mat2;
    uint64_t tmat;
    for (int i = 0; i < total_num; i++) {
        fr.get(&mat1, &mat2, &tmat);
        status_tbl[i].mat1 = mat1;
        status_tbl[i].mat2 = mat2;
        status_tbl[i].tmat = tmat;
    }
    Buffer status_buffer(context,
                         CL_MEM_READ_ONLY,
                         total_num * sizeof(tinymt64wp_t));
    queue.enqueueWriteBuffer(status_buffer,
                             CL_TRUE,
                             0,
                             total_num * sizeof(tinymt64wp_t),
                             status_tbl);
    delete[] status_tbl;
#if defined(DEBUG)
    cout << "get_rec_buff end" << endl;
#endif
    return status_buffer;
}

/**
 * parsing command line options
 *@param argc number of arguments
 *@param argv array of argument strings
 *@return true if errors are found in command line arguments
 */
static bool parse_opt(int argc, char **argv) {
#if defined(DEBUG)
    cout << "parse_opt start" << endl;
#endif
    bool error = false;
    std::string pgm = argv[0];
    errno = 0;
    if (argc <= 4) {
        error = true;
    }
    while (!error) {
        filename = argv[1];
        group_num = strtol(argv[2], NULL, 10);
        if (errno) {
            error = true;
            cerr << "group num error!" << endl;
            cerr << strerror(errno) << endl;
            break;
        }
        local_num = strtol(argv[3], NULL, 10);
        if (errno) {
            error = true;
            cerr << "local num error!" << endl;
            cerr << strerror(errno) << endl;
            break;
        }
        data_count = strtol(argv[4], NULL, 10);
        if (errno) {
            error = true;
            cerr << "data count error!" << endl;
            cerr << strerror(errno) << endl;
            break;
        }
        if (!filename.empty()) {
            ifstream ifs(filename.c_str());
            if (ifs) {
                ifs.close();
            } else {
                error = true;
                cerr << "can't open file:" << filename << endl;
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
