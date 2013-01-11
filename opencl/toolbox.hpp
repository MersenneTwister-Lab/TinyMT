#ifndef TOOLBOX_HPP
#define TOOLBOX_HPP

//#include "cl.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

namespace toolbox {
    class SimpleToolBox {
	typedef std::pair<const char*, ::size_t> Lines;
	typedef std::vector<Lines> Sources;
    public:
	SimpleToolBox(cl_device_type type) {
	    using namespace cl;
	    cl_int success;
	    std::vector<Platform> platforms;
	    Platform::get(&platforms);
	    std::vector<Device> devices;
	    for (::size_t i = 0; i < platforms.size(); i++) {
		success = platforms[i].getDevices(type, &devices);
		if (success) {
		    break;
		}
	    }
	    device = devices[0];
	    Context x(type);
	    context = x;
	    CommandQueue y(context, device, CL_QUEUE_PROFILING_ENABLE);
	    queue = y;
	};
	void addHeader(std::string filename) {
	    readSource(source, filename);
	};
	bool compile (std::string filename, bool verbose = true) {
	    using namespace std;
	    using namespace cl;
	    readSource(source, filename);
	    Program pro(context, source);
	    program = pro;
	    std::vector<cl::Device> devices;
	    devices.push_back(device);
	    try {
		program.build(devices);
	    } catch(Error e) {
		if (verbose) {
		    cout << filename << endl;
		    for (int i = 0; i < devices.size(); i++) {
			program.getBuildInfo(devices[i],
					     CL_PROGRAM_BUILD_OPTIONS,
					     &message);
			cout << message << endl;
			program.getBuildInfo(devices[i],
					     CL_PROGRAM_BUILD_LOG, &message);
			cout << message << endl;
		    }
		}
		return false;
	    }
	    return true;
	};
	cl::Device& getDevice() {
	    return device;
	};
	cl::Context& getContext() {
	    return context;
	};
	cl::Program& getProgram() {
	    return program;
	};
	std::string& getMessage() {
	    return message;
	};
	cl::CommandQueue& getQueue() {
	    return queue;
	};
	int getMaxWorkItemSize(int dim) {
	    std::vector<size_t> vec;
	    device.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &vec);
	    return vec[dim];
	};
	int getMaxGroupSize() {
	    size_t size;
	    device.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &size);
	    return size;
	};
    private:
	cl::Device device;
	cl::Context context;
	cl::Program program;
	std::string message;
	cl::CommandQueue queue;

	Sources source;
	void readSource(Sources& source, const std::string& filename) {
	    using namespace cl;
	    using namespace std;

	    ifstream ifs(filename.c_str());
	    static std::string s;
	    const int buffer_size = 256;
	    char buffer[buffer_size +1];
	    memset(buffer, 0, buffer_size+1);
	    while(!ifs.eof()) {
		ifs.getline(buffer, buffer_size);
		s += buffer;
		s += '\n';
	    }
	    //cout << s;
	    Lines l(s.c_str(), s.size());
	    source.push_back(l);
	    ifs.close();
	};

    };
    static inline std::string getInfo(cl::Event& e) {
	cl_int ret;
	std::stringstream ss;

	e.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS, &ret);
	switch (ret) {
	case CL_QUEUED:
	    ss << "Queued";
	    break;
	case CL_SUBMITTED:
	    ss << "Submitted";
	    break;
	case CL_RUNNING:
	    ss << "Rumming";
	    break;
	case CL_COMPLETE:
	    ss << "Complete";
	    break;
	default:
	    ss << "error(" << ret << ")";
	}
	std::string str;
	ss >> str;
	return str;
    };
}
#if 0
    void platform_getinfo(std::vector<cl::Platform>& platforms) {
	using namespace cl;
	using namespace std;

	for (int i = 0; i < platforms.size(); i++) {
	    cout << "Platform " << i << endl;
	    std::string profile;
	    platforms[i].getInfo(CL_PLATFORM_PROFILE, &profile);
	    cout << "Profile:" << endl;
	    cout << profile << endl;
	    std::string version;
	    platforms[i].getInfo(CL_PLATFORM_VERSION, &version);
	    cout << "Version:";
	    cout << version << endl;
	    std::string name;
	    platforms[i].getInfo(CL_PLATFORM_NAME, &name);
	    cout << "Name:" << name << endl;
	    std::string vender;
	    platforms[i].getInfo(CL_PLATFORM_VENDOR, &vender);
	    cout << "Vender:" << vender << endl;
	    std::string extensions;
	    platforms[i].getInfo(CL_PLATFORM_EXTENSIONS, &extensions);
	    cout << "Extensions:" << extensions << endl;
	}
    }

    void device_getinfo(std::vector<cl::Device>& devices) {
	using namespace cl;
	using namespace std;

	for (int i = 0; i < devices.size(); i++) {
	    std::string name;
	    devices[i].getInfo(CL_DEVICE_NAME, &name);
	    cout << "name:" << name << endl;
	    std::vector< ::size_t > work_item_sizes;
	    devices[i].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &work_item_sizes);
	    cout << "work_item_size:";
	    for (int j = 0; j < work_item_sizes.size(); j++) {
		cout << work_item_sizes[j] << " ";
	    }
	    cout << endl;

	}
    }

    void build_getinfo(std::vector<cl::Device> devices, cl::Program& program) {
	using namespace cl;
	using namespace std;

	std::string message;
	for (int i = 0; i < devices.size(); i++) {
	    program.getBuildInfo(devices[i],
				 CL_PROGRAM_BUILD_OPTIONS, &message);
	    cout << message << endl;
	    program.getBuildInfo(devices[i],
				 CL_PROGRAM_BUILD_LOG, &message);
	    cout << message << endl;
	}
    }

    typedef pair<const char*, ::size_t> Lines;
    typedef std::vector<Lines> Sources;
    void read_source(cl::Sources& source, const std::string& filename) {
	using namespace cl;
	using namespace std;

	ifstream ifs(filename.c_str());
	static std::string s;
	const int buffer_size = 256;
	char buffer[buffer_size +1];
	memset(buffer, 0, buffer_size+1);
	while(!ifs.eof()) {
	    ifs.getline(buffer, buffer_size);
	    s += buffer;
	    s += '\n';
	}
	//cout << s;
	Lines l(s.c_str(), s.size());
	source.push_back(l);
	ifs.close();
    }

    void get_device_context(cl::Device& device,
			    cl::Context& context,
			    cl::cl_device_type type) {
	using namespace cl;
	using namespace std;

	std::vector<Platform> platforms;
	Platform::get(&platforms);
	std::vector<Device> devices;
	cl_int success;
	for(int i = 0; i < platforms.size(); i++) {
	    try {
		success = platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &devices);
		if (success == CL_SUCCESS) {
		    break;
		}
	    } catch (cl::Error e) {
	    }
	}
	if (success != CL_SUCCESS) {
	    throw "something";
	}
	for (int i = 0; i < devices.size(); i++) {
	}
    }
#endif
#endif
