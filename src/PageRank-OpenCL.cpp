#include "Parser.h"
#include "Timer.h"
#include <fstream>
#include <typeinfo>
#include <CL/cl.hpp>


template<typename T>
void sparseMatrixIteration(std::string path, int wgSize, double error);

int main(int argc, char** argv) {
	double error = 1e-5;
	int datatype = 1; // 0 = float, 1 = double
	int workgroupSize = 128;
	if(argc > 3) {
		error = atof(argv[1]);
		datatype = atoi(argv[2]);
		workgroupSize = atoi(argv[3]);
	}

	datatype == 0 ? sparseMatrixIteration<float>("graph-google.txt", workgroupSize, error) : sparseMatrixIteration<double>("graph-google.txt", workgroupSize, error);
}

template<typename T>
void sparseMatrixIteration(std::string path, int workgroupSize, double err) {
	auto matrixInfo = Parser::getSparseMatrix<T>(path);
	auto& M = matrixInfo.matrix;
	const int N = matrixInfo.idMap.size();
	const T initRank = 1.0 / N;
	const T d = 0.85;
	const T offset = (1 - d) / N;
	const int O = M.getData().size();

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
	try{
		unsigned int numGroups = ((unsigned int)((N - 1) / workgroupSize) + 1);
		
		std::ifstream in("src/shader.cl");
		ASSERT(!in.is_open(), "File not found");
		std::string kernel_code = std::string(std::istreambuf_iterator<char>(in), (std::istreambuf_iterator<char>())); //getShaderSource("src/shader.cl");
		std::string prepend = typeid(T).name()[0] == 'd' ? "#define USE_DOUBLE 1\n" : "#define USE_DOUBLE 0\n";
		kernel_code.insert(0, prepend);
		in.close();

		cl::Program::Sources sources(1, {kernel_code.c_str(), kernel_code.length()});
		cl::Program program(context, sources);
		ASSERT(program.build({device}) != CL_SUCCESS, program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device))
		int ret = program.build({device});

		// Create buffers on the device
		cl::Buffer newRank(context, CL_MEM_READ_WRITE, N*sizeof(T));
		cl::Buffer oldRank(context, CL_MEM_READ_WRITE, N*sizeof(T));
		cl::Buffer matrix(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, O*sizeof(SparseValue<T>), M.getData().data());
		cl::Buffer rankDiffBuf(context, CL_MEM_READ_WRITE, N*sizeof(T));
		cl::Buffer partials(context, CL_MEM_READ_WRITE, numGroups*sizeof(T));

		// Create queue to which we will push commands for the device.
		cl::CommandQueue queue(context, device);

		// Create kernel and set params
		cl::Kernel matrixKernel(program, "matrix", &ret);
		ret |= matrixKernel.setArg(0, newRank);
		ret |= matrixKernel.setArg(1, oldRank);
		ret |= matrixKernel.setArg(2, matrix);
		ret |= matrixKernel.setArg(3, O);

		cl::Kernel matrixUpdateKernel(program, "matrixUpdateRank");
		ret |= matrixUpdateKernel.setArg(0, newRank);
		ret |= matrixUpdateKernel.setArg(1, oldRank);
		ret |= matrixUpdateKernel.setArg(2, N);
		ret |= matrixUpdateKernel.setArg(3, d);
		ret |= matrixUpdateKernel.setArg(4, offset);
		ret |= matrixUpdateKernel.setArg(5, rankDiffBuf);

		cl::Kernel reductionKernel(program, "rankDiffReduction");
		ret |= reductionKernel.setArg(0, rankDiffBuf);
		ret |= reductionKernel.setArg(1, N);
		ret |= reductionKernel.setArg(2, partials);
		ret |= reductionKernel.setArg(3, workgroupSize*sizeof(T), NULL);

		unsigned int globalSize = ((unsigned int)((O - 1) / workgroupSize) + 1) * workgroupSize;
		unsigned int globalSizeUpdate = numGroups * workgroupSize;

		std::vector<T> nrank(N, 0);
		std::vector<T> rank(N, initRank);

		Timer t("Matrix");
		ret |= queue.enqueueWriteBuffer(oldRank, CL_TRUE, 0, N*sizeof(T), rank.data());
		ret |= queue.enqueueWriteBuffer(newRank, CL_TRUE, 0, N*sizeof(T), nrank.data());

		// Run the kernel
		T rankDiff = 1;
		T* diff = new T[numGroups];
		//T* ranks = new T[N];
		
		for (int counter = 0; rankDiff > err; counter++) {
			ret |= queue.enqueueNDRangeKernel(matrixKernel, cl::NullRange, cl::NDRange(globalSize), cl::NDRange(workgroupSize));
			ret |= queue.enqueueNDRangeKernel(matrixUpdateKernel, cl::NullRange, cl::NDRange(globalSizeUpdate), cl::NDRange(workgroupSize));
			ret |= queue.enqueueNDRangeKernel(reductionKernel, cl::NullRange, cl::NDRange(globalSizeUpdate), cl::NDRange(workgroupSize));
			//ret |= queue.enqueueReadBuffer(oldRank, CL_TRUE, 0, N*sizeof(T), ranks);
			ret |= queue.enqueueReadBuffer(partials, CL_TRUE, 0, numGroups*sizeof(T), diff);
			//for(int i = 0; i < N; i++) printf("%f\n", i, ranks[i]);
			rankDiff = 0;
			for(int i = 0; i < numGroups; i++){
				rankDiff += diff[i];
			}
			//std::cout << "[" << counter << "] " << rankDiff << std::endl;
		}
		queue.finish();
	}catch(std::string msg){
		std::cout << msg << std::endl;
	}
}
