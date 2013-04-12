/**
 * Test program for OpenCL
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
#include "tinymt32def.h"
#include "tinymt32.h"
#include "test_common.h"
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
static tinymt32_t * tinymt32;

static std::string filename;
static int group_num;
static int local_num;
static int data_count;

static bool parse_opt(int argc, char **argv);
static int init_check_data(tinymt32_t tinymt32[],
                           int total_num,
                           uint32_t seed);
static int init_check_data_array(tinymt32_t tinymt32[],
                                 int total_num,
                                 uint32_t seed_array[],
                                 int size);
static void check_data(uint32_t * h_data,
                       int num_data,
                       int total_num);
static void check_data12(float * h_data,
                         int num_data,
                         int total_num);
static void check_data01(float * h_data,
                         int num_data,
                         int total_num);
static void check_status(tinymt32wp_t * h_status,
                         int total_num);
static void initialize_by_seed(Buffer& tinymt_status,
                               int total_num,
                               int local_num,
                               uint32_t seed);
static void initialize_by_array(Buffer& tinymt_status,
                                int group,
                                int local_num,
                                uint32_t seed_array[],
                                int seed_size);
static void make_tinymt(std::string& filename, int total_num);
static Buffer get_param_buff(std::string& filename,
                             int total_num);
static void generate_uint32(Buffer& tinymt_status,
                            int total_num,
                            int local_num,
                            int data_size);
static void generate_single12(Buffer& tinymt_status,
                              int total_num,
                              int local_num,
                              int data_size);
static void generate_single01(Buffer& tinymt_status,
                              int total_num,
                              int local_num,
                              int data_size);
static int test(int argc, char * argv[]);

/* ========================= */
/* tinymt32 test code        */
/* ========================= */
/**
 * main
 * catch errors
 *@param argc number of arguments
 *@param argv array of arguments
 *@return 0 normal, -1 error
 */
int main(int argc, char * argv[])
{
    try {
        return test(argc, argv);
    } catch (Error e) {
        cerr << "Error Code:" << e.err() << endl;
        cerr << e.what() << endl;
    } catch (std::string& er) {
        cerr << er << endl;
    } catch (...) {
        cerr << "other error" << endl;
    }
    return -1;
}

/**
 * test main
 *@param argc number of arguments
 *@param argv array of arguments
 *@return 0 normal, -1 error
 */
static int test(int argc, char * argv[])
{
#if defined(DEBUG)
    cout << "test start" << endl;
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
    source = getSource("test32.cli");
#else
    source = getSource("test32.cl");
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
    Buffer tinymt_status = get_param_buff(filename, total_num);
    // initialize by seed
    // generate uint32_t
    tinymt32 = new tinymt32_t[total_num];
    make_tinymt(filename, total_num);
    init_check_data(tinymt32, total_num, 1234);
    initialize_by_seed(tinymt_status, total_num, local_num, 1234);
    for (int i = 0; i < 2; i++) {
        generate_uint32(tinymt_status, total_num,
                        local_num, data_count);
    }

    // initialize by array
    // generate single float
    uint32_t seed_array[5] = {1, 2, 3, 4, 5};
    make_tinymt(filename, total_num);
    init_check_data_array(tinymt32, total_num, seed_array, 5);
    initialize_by_array(tinymt_status, total_num,
                        local_num, seed_array, 5);
    for (int i = 0; i < 1; i++) {
        generate_single12(tinymt_status, total_num,
                          local_num, data_count);
        generate_single01(tinymt_status, total_num,
                          local_num, data_count);
    }
    delete[] tinymt32;
    return 0;
}

/**
 * initialize tinymt status in device global memory
 * using 1 parameter for 1 generator.
 *@param tinymt_status internal state of kernel side tinymt
 *@param total total number of work itmes
 *@param local_item number of local work items
 *@param seed seed for initialization
 */
static void initialize_by_seed(Buffer& tinymt_status,
                               int total,
                               int local_item,
                               uint32_t seed)
{
#if defined(DEBUG)
    cout << "initialize_by_seed start" << endl;
#endif
    Kernel init_kernel(program, "tinymt_init_seed_kernel");
    init_kernel.setArg(0, tinymt_status);
    init_kernel.setArg(1, seed);
    NDRange global(total);
    NDRange local(local_item);
    Event event;
#if defined(DEBUG)
    cout << "global:" << dec << total << endl;
    cout << "group:" << dec << (total / local_item) << endl;
    cout << "local:" << dec << local_item << endl;
#endif
    queue.enqueueNDRangeKernel(init_kernel,
                               NullRange,
                               global,
                               local,
                               NULL,
                               &event);
    double time = get_time(event);
    tinymt32wp_t status[total];
    queue.enqueueReadBuffer(tinymt_status,
                            CL_TRUE,
                            0,
                            sizeof(tinymt32wp_t) * total,
                            status);
    cout << "initializing time = " << time * 1000 << "ms" << endl;
#if defined(DEBUG)
    cout << "status[0].s0:" << hex << status[0].s0 << endl;
    cout << "status[0].s1:" << hex << status[0].s1 << endl;
    cout << "status[0].s2:" << hex << status[0].s2 << endl;
    cout << "status[0].s3:" << hex << status[0].s3 << endl;
#endif
    check_status(status, total);
#if defined(DEBUG)
    cout << "initialize_by_seed end" << endl;
#endif
}

/**
 * initialize tinymt status in device global memory
 * using 1 parameter for 1 generator.
 *@param tinymt_status internal state of kernel side tinymt
 *@param total total number of work itmes
 *@param local_item number of local work items
 *@param seed_array seeds for initialization
 *@param seed_size size of seed_array
 */
static void initialize_by_array(Buffer& tinymt_status,
                                int total,
                                int local_item,
                                uint32_t seed_array[],
                                int seed_size)
{
#if defined(DEBUG)
    cout << "initialize_by_array start" << endl;
#endif
    Buffer seed_array_buffer(context,
                             CL_MEM_READ_WRITE,
                             seed_size * sizeof(uint32_t));
    queue.enqueueWriteBuffer(seed_array_buffer,
                             CL_TRUE,
                             0,
                             seed_size * sizeof(uint32_t),
                             seed_array);
    Kernel init_kernel(program, "tinymt_init_array_kernel");
    init_kernel.setArg(0, tinymt_status);
    init_kernel.setArg(1, seed_array_buffer);
    init_kernel.setArg(2, seed_size);
    NDRange global(total);
    NDRange local(local_item);
    Event event;
    queue.enqueueNDRangeKernel(init_kernel,
                               NullRange,
                               global,
                               local,
                               NULL,
                               &event);
    double time = get_time(event);
    tinymt32wp_t status[total];
    queue.enqueueReadBuffer(tinymt_status,
                            CL_TRUE,
                            0,
                            sizeof(tinymt32wp_t) * total,
                            status);
    cout << "initializing time = " << time * 1000 << "ms" << endl;
    check_status(status, total);
#if defined(DEBUG)
    cout << "initialize_by_array end" << endl;
#endif
}

/**
 * generate 32 bit unsigned random numbers in device global memory
 *@param tinymt_status internal state of kernel side tinymt
 *@param total_num total number of work itmes
 *@param local_num number of local work items
 *@param data_size number of data to generate
 */
static void generate_uint32(Buffer& tinymt_status,
                            int total_num,
                            int local_num,
                            int data_size)
{
#if defined(DEBUG)
    cout << "generate_uint32 start" << endl;
#endif
    int min_size = total_num;
    if (data_size % min_size != 0) {
        data_size = (data_size / min_size + 1) * min_size;
    }
    Kernel uint_kernel(program, "tinymt_uint32_kernel");
    Buffer output_buffer(context,
                         CL_MEM_READ_WRITE,
                         data_size * sizeof(uint32_t));
    uint_kernel.setArg(0, tinymt_status);
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
    uint32_t * output = new uint32_t[data_size];
    generate_event.wait();
    queue.enqueueReadBuffer(output_buffer,
                            CL_TRUE,
                            0,
                            data_size * sizeof(uint32_t),
                            output);
    check_data(output, data_size, total_num);
#if defined(DEBUG)
    print_uint32(output, data_size, total_num);
#endif
    double time = get_time(generate_event);
    cout << "generate time:" << time * 1000 << "ms" << endl;
    delete[] output;
#if defined(DEBUG)
    cout << "generate_uint32 end" << endl;
#endif
}

/**
 * generate single precision floating point numbers in the range [1, 2)
 * in device global memory
 *@param tinymt_status internal state of kernel side tinymt
 *@param total_num total number of work itmes
 *@param local_num number of local work items
 *@param data_size number of data to generate
 */
static void generate_single12(Buffer& tinymt_status,
                              int total_num,
                              int local_num,
                              int data_size)
{
    int min_size = total_num;
    if (data_size % min_size != 0) {
        data_size = (data_size / min_size + 1) * min_size;
    }
    Kernel single_kernel(program, "tinymt_single12_kernel");
    Buffer output_buffer(context,
                         CL_MEM_READ_WRITE,
                         data_size * sizeof(float));
    single_kernel.setArg(0, tinymt_status);
    single_kernel.setArg(1, output_buffer);
    single_kernel.setArg(2, data_size / total_num);
    NDRange global(total_num);
    NDRange local(local_num);
    Event generate_event;
    queue.enqueueNDRangeKernel(single_kernel,
                               NullRange,
                               global,
                               local,
                               NULL,
                               &generate_event);
    float * output = new float[data_size];
    generate_event.wait();
    queue.enqueueReadBuffer(output_buffer,
                            CL_TRUE,
                            0,
                            data_size * sizeof(float),
                            &output[0]);
    check_data12(output, data_size, total_num);
#if defined(DEBUG)
    print_float(&output[0], data_size, total_num);
#endif
    double time = get_time(generate_event);
    delete[] output;
    cout << "generate time:" << time * 1000 << "ms" << endl;
}

/**
 * generate single precision floating point numbers in the range [0, 1)
 * in device global memory
 *@param tinymt_status internal state of kernel side tinymt
 *@param total_num total number of work itmes
 *@param local_num number of local work items
 *@param data_size number of data to generate
 */
static void generate_single01(Buffer& tinymt_status,
                              int total_num,
                              int local_num,
                              int data_size)
{
    int min_size = total_num;
    if (data_size % min_size != 0) {
        data_size = (data_size / min_size + 1) * min_size;
    }
    Kernel single_kernel(program, "tinymt_single01_kernel");
    Buffer output_buffer(context,
                         CL_MEM_READ_WRITE,
                         data_size * sizeof(float));
    single_kernel.setArg(0, tinymt_status);
    single_kernel.setArg(1, output_buffer);
    single_kernel.setArg(2, data_size / total_num);
    NDRange global(total_num);
    NDRange local(local_num);
    Event generate_event;
    queue.enqueueNDRangeKernel(single_kernel,
                               NullRange,
                               global,
                               local,
                               NULL,
                               &generate_event);
    float * output = new float[data_size];
    generate_event.wait();
    queue.enqueueReadBuffer(output_buffer,
                            CL_TRUE,
                            0,
                            data_size * sizeof(float),
                            &output[0]);
    check_data01(output, data_size, total_num);
#if defined(DEBUG)
    print_float(&output[0], data_size, local_num);
#endif
    double time = get_time(generate_event);
    delete[] output;
    cout << "generate time:" << time * 1000 << "ms" << endl;
}


/* ==============
 * check programs
 * ==============*/
static void make_tinymt(std::string& filename, int total_num)
{
    tinymt::file_reader fr(filename);
    tinymt32 = new tinymt32_t[total_num];
    uint32_t mat1;
    uint32_t mat2;
    uint32_t tmat;
    for (int i = 0; i < total_num; i++) {
        fr.get(&mat1, &mat2, &tmat);
        tinymt32[i].mat1 = mat1;
        tinymt32[i].mat2 = mat2;
        tinymt32[i].tmat = tmat;
    }
}

static int init_check_data(tinymt32_t tinymt32[],
                           int total_num,
                           uint32_t seed)
{
#if defined(DEBUG)
    cout << "init_check_data start" << endl;
#endif

    for (int i = 0; i < total_num; i++) {
        tinymt32_init(&tinymt32[i], seed + i);
    }
#if defined(DEBUG)
    cout << "init_check_data end" << endl;
#endif
    return 0;
}

static int init_check_data_array(tinymt32_t tinymt32[],
                                 int total_num,
                                 uint32_t seed_array[],
                                 int size)
{
#if defined(DEBUG)
    cout << "init_check_data_array start" << endl;
#endif
    for (int i = 0; i < total_num; i++) {
        tinymt32_init_by_array(&tinymt32[i], seed_array, size);
    }
#if defined(DEBUG)
    cout << "init_check_data_array end" << endl;
#endif
    return 0;
}

static void check_data(uint32_t * h_data,
                       int num_data,
                       int total_num)
{
#if defined(DEBUG)
    cout << "check_data start" << endl;
#endif
    int size = num_data / total_num;
#if defined(DEBUG)
    cout << "size = " << dec << size << endl;
#endif
    bool error = false;
    for (int i = 0; i < total_num; i++) {
        bool disp_flg = true;
        int count = 0;
        for (int j = 0; j < size; j++) {
            uint32_t r = tinymt32_generate_uint32(&tinymt32[i]);
            if ((h_data[j * total_num + i] != r) && disp_flg) {
                cout << "mismatch i = " << dec << i
                     << " j = " << dec << j
                     << " data = " << hex << h_data[j * total_num + i]
                     << " r = " << hex << r << endl;
                cout << "check_data check N.G!" << endl;
                count++;
                error = true;
            }
            if (count > 10) {
                disp_flg = false;
            }
        }
    }
    if (!error) {
        cout << "check_data check O.K!" << endl;
    } else {
        throw cl::Error(-1, "tinymt32 check_data error!");
    }
#if defined(DEBUG)
    cout << "check_data end" << endl;
#endif
}

static void check_data12(float * h_data,
                         int num_data,
                         int total_num)
{
#if defined(DEBUG)
    cout << "check_data start" << endl;
#endif
    int size = num_data / total_num;
#if defined(DEBUG)
    cout << "size = " << dec << size << endl;
#endif
    bool error = false;
    for (int i = 0; i < total_num; i++) {
        bool disp_flg = true;
        int count = 0;
        for (int j = 0; j < size; j++) {
            float r = tinymt32_generate_float12(&tinymt32[i]);
            float d = h_data[j * total_num + i];
            bool ok = (-FLT_EPSILON <= (r - d))
                && ((r - d) <= FLT_EPSILON);
            if (!ok && disp_flg) {
                cout << "mismatch i = " << dec << i
                     << " j = " << dec << j
                     << " data = " << dec << h_data[j * total_num + i]
                     << " r = " << dec << r << endl;
                cout << "check_data check N.G!" << endl;
                count++;
                error = true;
            }
            if (count > 10) {
                disp_flg = false;
            }
        }
    }
    if (!error) {
        cout << "check_data check O.K!" << endl;
    } else {
        throw cl::Error(-1, "tinymt32 check_data error!");
    }
#if defined(DEBUG)
    cout << "check_data end" << endl;
#endif
}

static void check_data01(float * h_data,
                         int num_data,
                         int total_num)
{
#if defined(DEBUG)
    cout << "check_data start" << endl;
#endif
    int size = num_data / total_num;
#if defined(DEBUG)
    cout << "size = " << dec << size << endl;
#endif
    bool error = false;
    for (int i = 0; i < total_num; i++) {
        bool disp_flg = true;
        int count = 0;
        for (int j = 0; j < size; j++) {
            float r = tinymt32_generate_float(&tinymt32[i]);
            float d = h_data[j * total_num + i];
            bool ok = (-FLT_EPSILON <= (r - d))
                && ((r - d) <= FLT_EPSILON);
            if (!ok && disp_flg) {
                cout << "mismatch i = " << dec << i
                     << " j = " << dec << j
                     << " data = " << dec << h_data[j * total_num + i]
                     << " r = " << dec << r << endl;
                cout << "check_data check N.G!" << endl;
                count++;
                error = true;
            }
            if (count > 10) {
                disp_flg = false;
            }
        }
    }
    if (!error) {
        cout << "check_data check O.K!" << endl;
    } else {
        throw cl::Error(-1, "tinymt32 check_data error!");
    }
#if defined(DEBUG)
    cout << "check_data end" << endl;
#endif
}

static void check_status(tinymt32wp_t * h_status,
                         int total_num)
{
#if defined(DEBUG)
    cout << "check_status start" << endl;
#endif
    typedef struct {
        uint32_t status[4];
        uint32_t mat1;
        uint32_t mat2;
        uint32_t tmat;
    } sp;
    sp * dummy = (sp *)h_status;
    int counter = 0;
#if defined(DEBUG)
        cout << "device:" << endl;
        cout << "s0:" << hex << h_status[0].s0 << endl;
        cout << "s1:" << hex << h_status[0].s1 << endl;
        cout << "s2:" << hex << h_status[0].s2 << endl;
        cout << "s3:" << hex << h_status[0].s3 << endl;
        cout << "mat1:" << hex << h_status[0].mat1 << endl;
        cout << "mat2:" << hex << h_status[0].mat2 << endl;
        cout << "tmat:" << hex << h_status[0].tmat << endl;
        cout << "host:" << endl;
        cout << "s0:" << hex << tinymt32[0].status[0] << endl;
        cout << "s1:" << hex << tinymt32[0].status[1] << endl;
        cout << "s2:" << hex << tinymt32[0].status[2] << endl;
        cout << "s3:" << hex << tinymt32[0].status[3] << endl;
        cout << "mat1:" << hex << tinymt32[0].mat1 << endl;
        cout << "mat2:" << hex << tinymt32[0].mat2 << endl;
        cout << "tmat:" << hex << tinymt32[0].tmat << endl;
#endif
    for (int i = 0; i < total_num; i++) {
        for (int j = 0; j < 4; j++) {
            uint32_t x = dummy[i].status[j];
            uint32_t r = tinymt32[i].status[j];
            if (j == 0) {
                x = x & TINYMT32_MASK;
                r = r & TINYMT32_MASK;
            }
#if defined(DEBUG)
            if (i == 0 && counter == 0) {
                cout << "i = " << dec << i
                     << " j = " << dec << j
                     << " device = " << hex << x
                     << " host = " << hex << r << endl;
            }
#endif
            if (x != r) {
                cout << "mismatch i = " << dec << i
                     << " j = " << dec << j
                     << " device = " << hex << x
                     << " host = " << hex << r << endl;
                cout << "check_status check N.G!" << endl;
                counter++;
            }
            if (counter > 10) {
                return;
            }
        }
    }
    if (counter == 0) {
        cout << "check_status check O.K!" << endl;
    } else {
        throw cl::Error(-1, "tinymt32 check_status error!");
    }
#if defined(DEBUG)
    cout << "check_status end" << endl;
#endif
}

/* ==============
 * utility programs
 * ==============*/
static Buffer get_param_buff(std::string& filename,
                             int total_num)
{
#if defined(DEBUG)
    cout << "get_rec_buff start" << endl;
#endif
    tinymt::file_reader fr(filename);
    tinymt32wp_t * status_tbl = new tinymt32wp_t[total_num];
    uint32_t mat1;
    uint32_t mat2;
    uint32_t tmat;
    for (int i = 0; i < total_num; i++) {
        fr.get(&mat1, &mat2, &tmat);
        status_tbl[i].mat1 = mat1;
        status_tbl[i].mat2 = mat2;
        status_tbl[i].tmat = tmat;
    }
    Buffer status_buffer(context,
                         CL_MEM_READ_ONLY,
                         total_num * sizeof(tinymt32wp_t));
    queue.enqueueWriteBuffer(status_buffer,
                             CL_TRUE,
                             0,
                             total_num * sizeof(tinymt32wp_t),
                             status_tbl);
    delete[] status_tbl;
#if defined(DEBUG)
    cout << "get_rec_buff end" << endl;
#endif
    return status_buffer;
}

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
