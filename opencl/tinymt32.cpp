#define __CL_ENABLE_EXCEPTIONS

#include <memory.h>
#if defined(APPLE) || defined(__MACOSX) || defined(__APPLE__)
#include <OpenCL/opencl.h>
#include "cl.hpp"
#else
#include <CL/cl.hpp>
#endif
#include <boost/shared_array.hpp>
#include "toolbox.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include "file_reader.h"
#include "parse_opt.h"

extern "C" {
typedef uint32_t uint;
#include "tinymt32cl.h"
}

using namespace std;
using namespace cl;
using namespace toolbox;
using namespace tinymt;
using namespace boost;

double get_time(Event& event) {
    event.wait();
    cl_ulong start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    cl_ulong end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    return (end - start) * 1.0e-9;
}

void initialize(SimpleToolBox& box, options& opt,
		Buffer& tiny_buffer, int size) {
    file_reader r(opt.paramfile);
    shared_array<tinymt32cl> tinymt32(new tinymt32cl[size]);

    for (int i = 0; i < size; i++) {
	r.get(&tinymt32[i].mat1, &tinymt32[i].mat2, &tinymt32[i].tmat);
    }
    box.getQueue().enqueueWriteBuffer(tiny_buffer,
				    CL_TRUE,
				    0,
				    size * sizeof(tinymt32cl),
				    &tinymt32[0]);
    Kernel init_kernel(box.getProgram(), "tinymt_uint32_init");
    init_kernel.setArg(0, tiny_buffer);
    NDRange global(size);
    NDRange local(opt.local_num);
    Event event;
    box.getQueue().enqueueNDRangeKernel(init_kernel,
				      NullRange,
				      global,
				      local,
				      NULL,
				      &event);
    double time = get_time(event);
    cout << "initilize time = " << time * 1000 << "ms" << endl;
}

void print_uint32(uint32_t data[], int size, int item_num) {
    int max_seq = 10;
    int max_item = 6;
    if (size / item_num < max_seq) {
	max_seq = size / item_num;
    }
    if (item_num < max_item) {
	max_item = item_num;
    }
    for (int i = 0; i < max_seq; i++) {
	for (int j = 0; j < max_item; j++) {
	    cout << setw(10) << dec << data[item_num * i + j] << " ";
	}
	cout << endl;
    }
}

void print_float(float data[], int size, int item_num) {
    int max_seq = 10;
    int max_item = 6;
    if (size / item_num < max_seq) {
	max_seq = size / item_num;
    }
    if (item_num < max_item) {
	max_item = item_num;
    }
    for (int i = 0; i < max_seq; i++) {
	for (int j = 0; j < max_item; j++) {
	    cout << setprecision(9) << setw(10)
		 << dec << left << setfill('0') << data[item_num * i + j] << " ";
	}
	cout << endl;
    }
}

void generate_uint32(SimpleToolBox& box, int group_num, int local_num,
		     Buffer& tiny_buffer, int data_size, bool simd) {
    int item_num = local_num * group_num;
    if (data_size % item_num != 0) {
	data_size = (data_size / item_num + 1) * item_num;
    }
    Kernel uint_kernel(box.getProgram(), "tinymt_uint32_kernel");
    Buffer output_buffer(box.getContext(),
			 CL_MEM_READ_WRITE,
			 data_size * sizeof(uint32_t));
    uint_kernel.setArg(0, tiny_buffer);
    uint_kernel.setArg(1, output_buffer);
    uint_kernel.setArg(2, data_size / item_num);
    NDRange global(item_num);
    NDRange local(local_num);
    if (simd) {
	global = NDRange(item_num / 4);
	local = NDRange(local_num / 4);
    }
    Event generate_event;
    box.getQueue().enqueueNDRangeKernel(uint_kernel,
				      NullRange,
				      global,
				      local,
				      NULL,
				      &generate_event);
    shared_array<uint32_t> output(new uint32_t[data_size]);
    generate_event.wait();
    box.getQueue().enqueueReadBuffer(output_buffer,
				     CL_TRUE,
				     0,
				     data_size * sizeof(uint32_t),
				     &output[0]);
    print_uint32(&output[0], data_size, item_num);
    double time = get_time(generate_event);
    cout << "generate time:" << time * 1000 << "ms" << endl;
}

void generate_single(SimpleToolBox& box, int group_num, int local_num,
		     Buffer& tiny_buffer, int data_size, bool simd) {
    int item_num = local_num * group_num;
    if (data_size % item_num != 0) {
	data_size = (data_size / item_num + 1) * item_num;
    }
    Kernel single_kernel(box.getProgram(), "tinymt_single_kernel");
    Buffer output_buffer(box.getContext(),
			 CL_MEM_READ_WRITE,
			 data_size * sizeof(float));
    single_kernel.setArg(0, tiny_buffer);
    single_kernel.setArg(1, output_buffer);
    single_kernel.setArg(2, data_size / item_num);
    NDRange global(item_num);
    NDRange local(local_num);
    if (simd) {
	global = NDRange(item_num / 4);
	local = NDRange(local_num / 4);
    }
    Event generate_event;
    box.getQueue().enqueueNDRangeKernel(single_kernel,
				      NullRange,
				      global,
				      local,
				      NULL,
				      &generate_event);
    shared_array<float> output(new float[data_size]);
    generate_event.wait();
    box.getQueue().enqueueReadBuffer(output_buffer,
				     CL_TRUE,
				     0,
				     data_size * sizeof(float),
				     &output[0]);
    print_float(&output[0], data_size, item_num);
    double time = get_time(generate_event);
    cout << "generate time:" << time * 1000 << "ms" << endl;
}

int test(int argc, char * argv[]) {
    options opt;
    if (!parse_opt(opt, argc, argv)) {
	return -1;
    }
    SimpleToolBox box(CL_DEVICE_TYPE_GPU);
    box.addHeader("./tinymt32cl.h");
    if (!box.compile(opt.sourcefile)) {
        return -1;
    }
    int max_group_size = box.getMaxGroupSize();
    if (opt.group_num > max_group_size) {
	cout << "group_num greater than max value(" << max_group_size << ")"
	     << endl;
	return -1;
    }
    int max_size = box.getMaxWorkItemSize(0);
    int size = opt.group_num * opt.local_num;
    if (opt.local_num > max_size) {
	cout << "local_num greater than max value("
	     << max_size << ")" << endl;
	return -1;
    }
    Buffer tiny_buffer(box.getContext(),
		       CL_MEM_READ_WRITE,
		       size * sizeof(tinymt32cl));
    initialize(box, opt, tiny_buffer, size);
    generate_uint32(box, opt.group_num, opt.local_num,
		    tiny_buffer, opt.data_count, opt.simd);
    generate_single(box, opt.group_num, opt.local_num,
		    tiny_buffer, opt.data_count, opt.simd);
    return 0;
}

int main(int argc, char * argv[]) {
    try {
	return test(argc, argv);
    } catch (Error e) {
	cerr << "Error Code:" << e.err() << endl;
	cerr << e.what() << endl;
    }
}
