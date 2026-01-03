#include <chrono>

class Timer{
    private:
    std::chrono::high_resolution_clock::time_point startTime;

    public:
    void StartTimer();

    int GetElapsed();

};
