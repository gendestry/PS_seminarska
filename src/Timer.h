#include <chrono>
#include <iostream>

// Basic timer class used for timing code sections
class Timer {
private:
	std::string name;
	std::chrono::system_clock::time_point time_start;
	bool done = false;

public:
	Timer(std::string name) : name(name) {
		time_start = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		if(!done)
			stop();
	}

	void stop() {
		auto time_stop = std::chrono::high_resolution_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(time_stop - time_start);
		printf("%.9f\n", time.count() / 1e9);
		done = true;
	}
};