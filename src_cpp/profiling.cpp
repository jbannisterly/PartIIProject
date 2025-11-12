#include "profiling.hpp"

void Timer::StartTimer() {
    startTime = std::chrono::high_resolution_clock::now();
};

int Timer::GetElapsed() {
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
};