#ifndef OPENCL_TOOLS_HPP
#define OPENCL_TOOLS_HPP

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define __CL_ENABLE_EXCEPTIONS

#if defined(APPLE) || defined(__MACOSX) || defined(__APPLE__)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <string>

/* ========================= */
/* OpenCL interface function */
/* ========================= */
extern std::vector<cl::Platform> platforms;
extern std::vector<cl::Device> devices;
extern cl::Context context;
extern std::string programBuffer;
extern cl::Program program;
extern cl::Program::Sources source;
extern cl::CommandQueue queue;
extern std::string errorMessage;

static inline std::vector<cl::Platform> getPlatforms()
{
    using namespace std;
    using namespace cl;
    std::vector<cl::Platform> platforms;
#if defined(DEBUG)
    cout << "start get platform" << endl;
#endif
    errorMessage = "getPlatform failed";
    Platform::get(&platforms);
#if defined(DEBUG)
    cout << "vendor:" << endl;
    for (unsigned int i = 0; i < platforms.size(); i++) {
        cout << platforms[i].getInfo<CL_PLATFORM_VENDOR>() << endl;
    }
#endif
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get platform" << endl;
#endif
    return platforms;
}

static inline std::vector<cl::Device> getDevices()
{
    using namespace std;
    using namespace cl;
    std::vector<cl::Device> devices;
#if defined(DEBUG)
    cout << "start get devices" << endl;
#endif
    errorMessage = "getDevices failed";
    platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get devices" << endl;
#endif
    return devices;
}

static inline cl::Context getContext()
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start get context" << endl;
#endif
    errorMessage = "create context failed";
    Context context(devices);
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get context" << endl;
#endif
    return context;
}

static inline cl::Program::Sources getSource(const char * filename)
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start getSource" << endl;
#endif
    ifstream ifs;
    errorMessage = "getSource failed";
    ifs.open(filename, fstream::in | fstream::binary);
    if (ifs) {
        ifs.seekg(0, std::fstream::end);
        ::size_t size = (::size_t)ifs.tellg();
        ifs.seekg(0, std::fstream::beg);
        char * buf = new char[size + 1];
        ifs.read(buf, size);
        ifs.close();
        buf[size] = '\0';
        programBuffer = buf;
        delete[] buf;
    } else {
        errorMessage = "open file failed";
        throw Error(ifs.rdstate(), "open file failed");
    }
    errorMessage = "create sources failed";
    cl::Program::Sources source(1,
                                make_pair(programBuffer.c_str(),
                                          programBuffer.size()));
    errorMessage = "";
#if defined(DEBUG)
    cout << "end getSource" << endl;
#endif
    return source;
}

static inline bool hasDoubleExtension()
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start has double extension" << endl;
#endif
    std::string extensions;
    cl_int err;
    errorMessage = "device getinfo(CL_DEVICE_EXTENSIONS) failed";
    err = devices[0].getInfo(CL_DEVICE_EXTENSIONS, &extensions);
    if (err != CL_SUCCESS) {
        cout << "device getinfo(CL_DEVICE_EXTENSIONS) err:"
             << dec << err << endl;
    }
    errorMessage = "";
#if defined(DEBUG)
    cout << "device extensions:" << extensions << endl;
#endif
    if (extensions.find("cl_khr_fp64",0) != std::string::npos) {
        return true;
    } else {
        return false;
    }
}

static inline cl::Program getProgram(const char * option = "")
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start get program" << endl;
#endif
    cl_int err = 0;
    errorMessage = "create program failed";
    Program program = Program(context, source, &err);
#if defined(DEBUG)
    cout << "start build" << endl;
#endif
    errorMessage = "program build failed";
    try {
        err = program.build(devices, option);
    } catch (cl::Error e) {
        if (e.err() != CL_SUCCESS) {
            if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
                std::string str
                    = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
                cout << "compile error:" << endl;
                cout << str << endl;
            } else {
                cout << "build error but not program failure err:"
                     << dec << e.err()
                     << " " << e.what() << endl;
            }
        }
        throw e;
    }
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get program" << endl;
#endif
    return program;
}

static inline cl::CommandQueue getCommandQueue()
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start get command queue" << endl;
#endif
    cl_int err = 0;
    errorMessage = "create command queue failed";
    CommandQueue queue(context,
                       devices[0],
                       CL_QUEUE_PROFILING_ENABLE,
                       &err);
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get command queue" << endl;
#endif
    return queue;
}

static inline int getMaxGroupSize()
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start get max group size" << endl;
#endif
    ::size_t size;
    cl_int err;
    errorMessage = "device getinfo(CL_DEVICE_MAX_WORK_GROUP_SIZE) failed";
    err = devices[0].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &size);
    if (err != CL_SUCCESS) {
        cout << "device getinfo(CL_DEVICE_MAX_WORK_GROUP_SIZE) err:"
             << dec << err << endl;
    }
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get max group size" << endl;
#endif
    return size;
}

static inline cl_ulong getLocalMemSize()
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start get local mem size" << endl;
#endif
    cl_int err;
    cl_ulong size;
    errorMessage = "device getinfo(CL_DEVICE_LOCAL_MEM_SIZE) failed";
    err = devices[0].getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &size);
    if (err != CL_SUCCESS) {
        cout << "device getinfo(CL_DEVICE_LOCAL_MEM_SIZE) err:"
             << dec << err << endl;
    }
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get local mem size" << endl;
#endif
    return size;
}

static inline cl_ulong getConstantMemSize()
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start get constant mem size" << endl;
#endif
    cl_int err;
    cl_ulong size;
    errorMessage = "device getinfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE) failed";
    err = devices[0].getInfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &size);
    if (err != CL_SUCCESS) {
        cout << "device getinfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE) err:"
             << dec << err << endl;
    }
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get constant mem size" << endl;
#endif
    return size;
}

static inline cl_ulong getGlobalMemSize()
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start get global mem size" << endl;
#endif
    cl_int err;
    cl_ulong size;
    errorMessage = "device getinfo(CL_DEVICE_GLOBAL_MEM_SIZE) failed";
    err = devices[0].getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &size);
    if (err != CL_SUCCESS) {
        cout << "device getinfo(CL_DEVICE_GLOBAL_MEM_SIZE) err:"
             << dec << err << endl;
    }
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get global mem size" << endl;
#endif
    return size;
}

static inline int getMaxWorkItemSize(int dim)
{
    using namespace std;
    using namespace cl;
#if defined(DEBUG)
    cout << "start get max work item size" << endl;
#endif
    std::vector<std::size_t> vec;
    cl_int err;
    errorMessage = "device getinfo(CL_DEVICE_MAX_WORK_ITEM_SIZE) failed";
    err = devices[0].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &vec);
    if (err != CL_SUCCESS) {
        cout << "device getinfo(CL_DEVICE_MAX_WORK_ITEM_SIZES) err:"
             << dec << err << endl;
    }
    errorMessage = "";
#if defined(DEBUG)
    cout << "end get max work item size :" << dec << vec[dim] << endl;
#endif
    return vec[dim];
}

static inline double get_time(cl::Event& event)
{
    using namespace std;
    using namespace cl;
    event.wait();
    cl_ulong start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    cl_ulong end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    return (end - start) * 1.0e-9;
}

#endif /* OPENCL_TOOLS_HPP */
