#include "Parser.h"
#include "Timer.h"
#include <fstream>
#include <CL/cl.hpp>

std::string getShaderSource(std::string path);

int main(int argc, char** argv) {
    // Getting platforms
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    cl::Platform platform = all_platforms[0];
    
    // Getting devices
    std::vector<cl::Device> all_devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);
    cl::Device device = all_devices[0];
    cl::Context context(device);

    // Creating program
	std::string kernel_code = getShaderSource("shader.cl");
    cl::Program::Sources sources(1, {kernel_code.c_str(), kernel_code.length()});

    cl::Program program(context, sources);
    ASSERT(program.build({device}) != CL_SUCCESS, program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device))
    
    // Create buffers on the device
    // cl::Buffer outBuffer(context, CL_MEM_WRITE_ONLY, buffer_size);
    // cl::ImageFormat imageFormat = cl::ImageFormat(CL_RGBA, CL_UNSIGNED_INT8);
    // cl::Image2D outputImage = cl::Image2D(context, CL_MEM_WRITE_ONLY, imageFormat, WIDTH, HEIGHT);
 
    // Create queue to which we will push commands for the device.
    cl::CommandQueue queue(context, device);
 
    // Create kernel and set params
    cl::Kernel kernel_mandelbrot(program, "matrix");
    // kernel_mandelbrot.setArg(0, outputImage);

    // Run the kernel
    // queue.enqueueNDRangeKernel(kernel_mandelbrot, cl::NullRange, cl::NDRange(WIDTH, HEIGHT), cl::NullRange);
    queue.finish(); 

    return 0;
}

std::string getShaderSource(std::string path) {
	std::ifstream in(path);
    ASSERT(in.is_open(), "File not found");
    return std::string(std::istreambuf_iterator<char>(in), (std::istreambuf_iterator<char>()));
}