/**
 * Test host program to generate a sequence
 * using jump and parallel generation.
 *
 * 1. make initial state from a seed
 * 2. calculate initial position for each work group.
 *    (This step may consume many time)
 * 3. Loop:
 *   3.1 generate sub-sequences parallel
 *   3.2 jump for next loop
 */

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define __CL_ENABLE_EXCEPTIONS

#include <cstddef>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <float.h>
#include <stdint.h>
#include <inttypes.h>

#include "opencl_tools.hpp"
#include "tinymt32def.h"
#include "tinymt32.h"
#include "test_common.h"
#include "jump32.h"

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

/* ========================= */
/* global variables          */
/* ========================= */
static cl_int group_num;
static cl_int local_num;
static cl_long data_count;
static tinymt32_t tiny;
static const char * tinymt32j_characteristic = "d8524022ed8dff4a8dcc50c798faba43";
/* =========================
   declaration
   ========================= */
static int test(int argc, char * argv[]);
static int init_check_data(tinymt32_t * tiny,
                           uint32_t seed);
static Buffer make_jump_table_buffer(cl_ulong total_num, uint64_t step);
static Buffer make_loop_jump_table_buffer(cl_ulong total_num, uint64_t setp);
static Buffer get_status_buff(int total_num);
static void initialize_by_seed(Buffer& tinymt_status,
                               Buffer& jump_table_buffer,
                               int local_num,
                               int total_num,
                               uint32_t seed);
static void status_jump(Buffer& status_buffer,
                        Buffer& jump_buffer,
                        int local_num,
                        int total_num);
static void generate_uint32(Buffer& status_buffer,
                            int local_num,
                            int total_num,
                            int data_size);
static void check_data(uint32_t * h_data, int num_data, int total_num);
static bool parse_opt(int argc, char **argv);

/* =========================
   tiny test code
   ========================= */
/**
 * main
 * catch errors
 *@param argc number of arguments
 *@param argv array of arguments
 *@return 0 normal, -1 error
 */
int main(int argc, char * argv[]) {
    try {
        return test(argc, argv);
    } catch (cl::Error e) {
        cerr << "Error Code:" << e.err() << endl;
        cerr << errorMessage << endl;
        cerr << e.what() << endl;
    } catch (std::string& err) {
        cerr << err << endl;
    } catch (...) {
        cerr << "other errors" << endl;
    }
    return -1;
}
/**
 * test main
 *@param argc number of arguments
 *@param argv array of arguments
 *@return 0 normal, -1 error
 */
static int test(int argc, char * argv[]) {
#if defined(DEBUG)
    cout << "test start" << endl;
#endif
    if (!parse_opt(argc, argv)) {
        return -1;
    }
    int total_num = local_num * group_num;
    // OpenCL setup
#if defined(DEBUG)
    cout << "openCL setup start" << endl;
#endif
    platforms = getPlatforms();
    devices = getDevices();
    context = getContext();
#if defined(INCLUDE_IMPOSSIBLE)
    source = getSource("test32_jump2.cli");
#else
    source = getSource("test32_jump2.cl");
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

    int max_group_size = getMaxGroupSize();
    if (group_num > max_group_size) {
        cout << "group_num greater than max value("
             << max_group_size << ")"
             << endl;
        return -1;
    }
    int max_size = getMaxWorkItemSize(0);
    if (local_num > max_size) {
        cout << "workitem size is greater than max value("
             << dec << max_size << ")"
             << "current:" << dec << local_num << endl;
        return -1;
    }
    uint64_t max_data_per_loop
        = getGlobalMemSize() / 2 / total_num / sizeof(uint32_t);
    uint64_t jump_step = max_data_per_loop / total_num;
    max_data_per_loop = jump_step * total_num;
    Buffer jump_table_buffer
        = make_jump_table_buffer(group_num * local_num, jump_step);
    Buffer loop_jump_buffer
        = make_loop_jump_table_buffer(group_num * local_num, jump_step);
#if defined(DEBUG)
    cout << "jump step:" << dec << jump_step << endl;
    cout << "max_data_per_loop:" << dec << max_data_per_loop << endl;
#endif
    // initialize by seed
    // generate uint32_t
    init_check_data(&tiny, 1234);
    Buffer tinymt_status = get_status_buff(total_num);
    initialize_by_seed(tinymt_status, jump_table_buffer,
                       local_num, total_num, 1234);
    while (data_count > 0) {
        generate_uint32(tinymt_status, local_num, total_num, max_data_per_loop);
        status_jump(tinymt_status, loop_jump_buffer, local_num, total_num);
        data_count -= max_data_per_loop;
    }
    return 0;
}

/**
 * prepare table of jump polynomial.
 * @param total_num total number of work items
 * @param step jump step
 * @return buffer of jump table
 */
static Buffer make_jump_table_buffer(cl_ulong total_num, uint64_t step)
{
#if defined(DEBUG)
    cout << "make_jump_table_buffer start" << endl;
#endif
    uint64_t jump_table_size = 0;
    uint64_t mask = total_num;
    for (int i = 0; i < 64; i++) {
        if (mask & 1) {
            jump_table_size = i + 1;
        }
        mask = mask >> 1;
    }
    if (jump_table_size == 0) {
        cout << "error jump_tabale_size is zero." << endl;
        throw "jump_table_size is zero";
    }
    if (jump_table_size * 4 * sizeof(uint32_t) > getConstantMemSize()) {
        cout << "error jump_tabale_size greater"
             << " than max constant buffer size." << endl;
        throw "jump table size too large";
    }
    uint32_t *jump_table = new uint32_t[4 * jump_table_size];
    uint64_t ustep = 0;
    f2_polynomial jump_poly;
    for (uint64_t i = 0; i < jump_table_size; i++) {
#if defined(DEBUG)
        cout << " step:" << dec << step << endl;
        cout << "ustep:" << dec << ustep << endl;
#endif
        calculate_jump_polynomial(&jump_poly,
                                  step,
                                  ustep,
                                  tinymt32j_characteristic);
        ustep = (ustep << 1) | (step >> 63);
        step = step << 1;
        for (int j = 0; j < 2; j++) {
#if defined(DEBUG)
            cout << " index = [" << dec << i * 4 + j * 2;
            cout << "," << dec << i * 4 + j * 2 + 1;
            cout << "]" << endl;
#endif
            jump_table[i * 4 + j * 2]
                = static_cast<uint32_t>(jump_poly.ar[j]);
            jump_table[i * 4 + j * 2 + 1]
                = static_cast<uint32_t>(jump_poly.ar[j] >> 32);
        }
    }
#if defined(DEBUG)
    cout << "jump_table_size:" << dec << jump_table_size << endl;
    cout << "jump_table:" << endl;
    for (uint64_t i = 0; i < jump_table_size; i++) {
        for (int j = 0; j < 4; j++) {
            cout << hex << jump_table[i * 4 + j];
            cout << " ";
        }
        cout << endl;
    }
#endif
    // jump table
#if defined(DEBUG)
    cout << "start to make jump table buffer" << endl;
#endif
    Buffer buffer(context,
                  CL_MEM_READ_ONLY,
                  4 * jump_table_size * sizeof(uint32_t));
#if defined(DEBUG)
    cout << "start to copy jump table buffer" << endl;
#endif
    queue.enqueueWriteBuffer(buffer,
                             CL_TRUE,
                             0,
                             4 * jump_table_size * sizeof(uint32_t),
                             jump_table);
#if defined(DEBUG)
    cout << "make_jump_table_buffer end" << endl;
#endif
    delete[] jump_table;
    return buffer;
}

/**
 * prepare a jump polynomial for specified step
 *@param total_num total number of work items
 *@param step jump step
 *@return buffer of a jump polynomial
 */
static Buffer make_loop_jump_table_buffer(cl_ulong total_num, uint64_t step)
{
#if defined(DEBUG)
    cout << "make_long_jump_table_buffer start" << endl;
    cout << "jump status step:" << dec << (step * (total_num - 1))
         << endl;
#endif
    uint32_t loop_jump[4];
    f2_polynomial jump_poly;
    calculate_jump_polynomial(&jump_poly,
                              step * (total_num - 1),
                              0,
                              tinymt32j_characteristic);
    for (int i = 0; i < 2; i++) {
        loop_jump[i * 2]
            = static_cast<uint32_t>(jump_poly.ar[i]);
        loop_jump[i * 2 + 1]
            = static_cast<uint32_t>(jump_poly.ar[i] >> 32);
    }
    Buffer buffer(context,
                  CL_MEM_READ_ONLY,
                  4 * sizeof(uint32_t));
    queue.enqueueWriteBuffer(buffer,
                             CL_TRUE,
                             0,
                             4 * sizeof(uint32_t),
                             loop_jump);
#if defined(DEBUG)
    cout << "loop_jump:" << endl;
    for (int i = 0; i < 4; i++) {
        cout << hex << loop_jump[i];
        cout << " ";
    }
    cout << endl;
    cout << "make_loop_jump_table_buffer end" << endl;
#endif
    return buffer;
}

/**
 * initialize tinymt status in device global memory
 * using seed and fixed jump.
 * jump step is fixed to 3^40.
 *@param status_buffer tinymt status in device global memory
 *@param jump_table_buffer jump table in constant memory
 *@param local_num number of local work items.
 *@param total_num total number work items.
 *@param seed seed for initialization
 */
static void initialize_by_seed(Buffer& status_buffer,
                               Buffer& jump_table_buffer,
                               int local_num,
                               int total_num,
                               uint32_t seed)
{
#if defined(DEBUG)
    cout << "initialize_by_seed start" << endl;
#endif
    Kernel init_kernel(program, "tinymt_init_seed_kernel");
    init_kernel.setArg(0, status_buffer);
    init_kernel.setArg(1, jump_table_buffer);
    init_kernel.setArg(2, seed);
    NDRange global(total_num);
    NDRange local(local_num);
    Event event;
#if defined(DEBUG)
    cout << "global:" << dec << total_num << endl;
    cout << "local:" << dec << local_num << endl;
#endif
    queue.enqueueNDRangeKernel(init_kernel,
                               NullRange,
                               global,
                               local,
                               NULL,
                               &event);
    double time = get_time(event);
    cout << "initializing time = " << time * 1000 << "ms" << endl;
#if defined(DEBUG)
    cout << "initialize_by_seed end" << endl;
#endif
}

/**
 * jump tinymt status in device global memory
 *@param status_buffer tinymt status in device global memory
 *@param jump_table_buffer buffer of a jump polynomial
 *@param local_num number of local work items
 *@param total_num total number of work items
 */
static void status_jump(Buffer& status_buffer,
                        Buffer& jump_table_buffer,
                        int local_num,
                        int total_num)
{
#if defined(DEBUG)
    cout << "status jump start" << endl;
#endif
    Kernel jump_kernel(program, "tinymt_jump_kernel");
    jump_kernel.setArg(0, status_buffer);
    jump_kernel.setArg(1, jump_table_buffer);
    NDRange global(total_num);
    NDRange local(local_num);
    Event event;
#if defined(DEBUG)
    cout << "global:" << dec << total_num << endl;
    cout << "local:" << dec << local_num << endl;
#endif
    queue.enqueueNDRangeKernel(jump_kernel,
                               NullRange,
                               global,
                               local,
                               NULL,
                               &event);
    double time = get_time(event);
    cout << "jump time = " << time * 1000 << "ms" << endl;
#if defined(DEBUG)
    cout << "status jump end" << endl;
#endif
}

/**
 * generate 32 bit unsigned random numbers in device global memory
 *@param status_buffer internal state of kernel side tinymt
 *@param local_num number of local work items
 *@param total_num total number of work items
 *@param data_size number of data to generate
 */
static void generate_uint32(Buffer& status_buffer,
                            int local_num,
                            int total_num,
                            int data_size)
{
#if defined(DEBUG)
    cout << "generate_uint32 start" << endl;
#endif
    Kernel uint_kernel(program, "tinymt_uint32_kernel");
    Buffer output_buffer(context,
                         CL_MEM_READ_WRITE,
                         data_size * sizeof(uint32_t));
    uint_kernel.setArg(0, status_buffer);
    uint_kernel.setArg(1, output_buffer);
    uint_kernel.setArg(2, data_size / total_num);
    NDRange global(total_num);
    NDRange local(local_num);
    Event generate_event;
#if defined(DEBUG)
    cout << "generate_uint32 enque kernel start" << endl;
#endif
    queue.enqueueNDRangeKernel(uint_kernel,
                               NullRange,
                               global,
                               local,
                               NULL,
                               &generate_event);
#if defined(DEBUG)
    cout << "generate_uint32 enque kernel end" << endl;
#endif
    uint32_t * output = new uint32_t[data_size];
#if defined(DEBUG)
    cout << "generate_uint32 event wait start" << endl;
#endif
    generate_event.wait();
#if defined(DEBUG)
    cout << "generate_uint32 event wait end" << endl;
#endif
#if defined(DEBUG)
    cout << "generate_uint32 readbuffer start" << endl;
    cout << "data_size:" << dec << data_size << endl;
#endif
    queue.enqueueReadBuffer(output_buffer,
                            CL_TRUE,
                            0,
                            data_size * sizeof(uint32_t),
                            &output[0]);
#if defined(DEBUG)
    cout << "generate_uint32 readbuffer end" << endl;
#endif
    check_data(output, data_size, total_num);
#if defined(DEBUG)
    print_uint32(&output[0], data_size, total_num);
#endif
    double time = get_time(generate_event);
    cout << "generate time:" << time * 1000 << "ms" << endl;
    delete[] output;
#if defined(DEBUG)
    cout << "generate_uint32 end" << endl;
#endif
}

/* ==============
 * check programs
 * ==============*/

/**
 * initialize host side tinymt structure for check
 *@param tiny host side tinymt
 *@param seed seed for initialization
 *@return 0 if normal end
 */
static int init_check_data(tinymt32_t * tiny,
                           uint32_t seed)
{
#if defined(DEBUG)
    cout << "init_check_data start" << endl;
#endif
    tiny->mat1 = TINYMT32J_MAT1;
    tiny->mat2 = TINYMT32J_MAT2;
    tiny->tmat = TINYMT32J_TMAT;
    tinymt32_init(tiny, seed);
#if defined(DEBUG)
    cout << "init_check_data end" << endl;
#endif
    return 0;
}


/**
 * compare host side generation and kernel side generation
 *@param h_data host side copy of numbers generated by kernel side
 *@param num_data size of h_data
 *@param total_num total number of work items
 */
static void check_data(uint32_t * h_data, int num_data, int total_num)
{
#if defined(DEBUG)
    cout << "check_data start" << endl;
#endif
    bool error = false;
    bool disp_flg = true;
    int count = 0;
    int data_per_thread = num_data / total_num;
    int j;
    for (j = 0; j < num_data; j++) {
        uint32_t r = tinymt32_generate_uint32(&tiny);
        if ((h_data[j] != r) && disp_flg) {
            cout << "mismatch"
                 << " j = " << dec << j
                 << " data = " << hex << h_data[j]
                 << " r = " << hex << r << endl;
            cout << "check_data check N.G!" << endl;
            cout << "data_per_thread:" << dec << data_per_thread << endl;
            count++;
            error = true;
        }
        if (count > 10) {
            disp_flg = false;
        }
    }
    if (!error) {
        cout << "check_data check O.K!" << endl;
        cout << "count = " << dec << j << endl;
    } else {
        throw cl::Error(-1, "tinymt32 check_data error!");
    }
#if defined(DEBUG)
    cout << "check_data end" << endl;
#endif
}

/**
 * get buffer for kernel side tinymt
 *@param total_num total number of work items
 *@return buffer for kernel side tinymt
 */
static Buffer get_status_buff(int total_num)
{
#if defined(DEBUG)
    cout << "get_status_buff start" << endl;
#endif
    Buffer status_buffer(context,
                         CL_MEM_READ_WRITE,
                         total_num * sizeof(tinymt32j_t));
#if defined(DEBUG)
    cout << "get_status_buff end" << endl;
#endif
    return status_buffer;
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

