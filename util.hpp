#pragma once

#include<chrono>
#include<random>

namespace util {

class Timer {
	std::chrono::time_point<std::chrono::high_resolution_clock> start_;
	long long ns_ = 0;

public:

	Timer() {}

	Timer& start() {
		start_ = std::chrono::high_resolution_clock::now();
		return *this;
	}

	Timer& end() {
		auto end = std::chrono::high_resolution_clock::now();
		ns_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count();
		return *this;
	}

	double ms() {
		return ns_ * 0.000001;
	}
};

class Random {
        std::mt19937 generator;
        std::uniform_int_distribution<> dist_int;
        std::uniform_real_distribution<> dist_real;

    public:
        Random() : generator(std::random_device()()) {}
        
        void set_integer_bounds(int min, int max) {
            dist_int = std::uniform_int_distribution<>(min, max);
        }

        void set_real_bounds(double min, double max) {
            dist_real = std::uniform_real_distribution<>(min, max);
        }

        int integer() {
            return dist_int(generator);
        }

        double real() {
            return dist_real(generator);
        }
    };
}