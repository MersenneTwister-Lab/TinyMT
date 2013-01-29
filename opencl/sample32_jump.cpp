/**
 * Test program for OpenCL
 * using 1 parameter for 1 generator
 */
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define __CL_ENABLE_EXCEPTIONS

#include "opencl_tools.hpp"
typedef uint32_t uint;
#include "tinymt32def.h"

#include <cstddef>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <float.h>

typedef uint32_t uint;
#include "tinymt32.h"
#include "test_common.h"
#include "parse_opt.h"

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
    options opt;
    if (!parse_opt(opt, argc, argv)) {
	return -1;
    }
    // OpenCL setup
#if defined(DEBUG)
    cout << "openCL setup start" << endl;
#endif
    platforms = getPlatforms();
    devices = getDevices();
    context = getContext();
#if defined(APPLE) || defined(__MACOSX) || defined(__APPLE__)
    source = getSource("sample32_jump.cli");
#else
    source = getSource("sample32_jump.cl");
#endif
    const char * option = "";
#if defined(DEBUG)
    option = "-DDEBUG";
#endif
    program = getProgram(option);
    queue = getCommandQueue();
#if defined(DEBUG)
    cout << "openCL setup end" << endl;
#endif
    int total_num = opt.group_num * opt.local_num;
    int max_group_size = getMaxGroupSize();
    if (opt.group_num > max_group_size) {
	cout << "group_num greater than max value("
	     << max_group_size << ")"
	     << endl;
	return -1;
    }
    calc_pi(total_num, opt.local_num, opt.data_count);
    return 0;
}

/**
 * calculate PI using Monte Carlo Simulation
 *@param tinymt_status device global memories
 *@param total_num number of groups for execution
 *@param data_size number of data to generate
 */
static void calc_pi(int total_num,
		    int local_num,
		    int data_size)
{
#if defined(DEBUG)
    cout << "calc_pi start" << endl;
#endif
    int min_size = total_num;
    if (data_size % min_size != 0) {
	data_size = (data_size / min_size + 1) * min_size;
    }
    uint32_t seed = 1234;
    Kernel uint_kernel(program, "calc_pi");
    Buffer result_buffer(context,
			 CL_MEM_READ_WRITE,
			 sizeof(float));
    Buffer global_buffer(context,
			 CL_MEM_READ_WRITE,
			 sizeof(uint32_t) * total_num / local_num);

    uint_kernel.setArg(0, seed);
    uint_kernel.setArg(1, data_size / total_num);
    uint_kernel.setArg(2, result_buffer);
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
    float * result = new float[1];
    generate_event.wait();
    queue.enqueueReadBuffer(result_buffer,
			    CL_TRUE,
			    0,
			    sizeof(float),
			    result);
    double time = get_time(generate_event);
    cout << "generate time:" << time * 1000 << "ms" << endl;
    cout << "calculated pi = " << result[0] << endl;
    delete[] result;
#if defined(DEBUG)
    cout << "calc_pi end" << endl;
#endif
}

