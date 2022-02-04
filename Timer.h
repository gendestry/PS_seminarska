#include <chrono>
#include <iostream>
class Timer {
	std::string name;
	std::chrono::system_clock::time_point time_start;
public:
	Timer(std::string name) : name(name) {
		time_start = std::chrono::high_resolution_clock::now();
	}
	~Timer() {
		auto time_stop = std::chrono::high_resolution_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::microseconds>(time_stop - time_start);
		std::cout << name << ": " << time.count()/1e6 << std::endl;
	}
};