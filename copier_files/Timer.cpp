
#include "Timer.h"

Timer::Timer(shared_ptr<Writer> thewriter, shared_ptr<Reader> thereader) : thewriter{thewriter}, thereader{thereader}{
    timeLog = 0;
}

void Timer::run() {

    thereader->run();
}

void Timer::runTimed() {
    auto duration = [](clock_t start, clock_t end){return (double)(end - start) / (double)CLOCKS_PER_SEC;};

    clock_t start = clock();
    thereader->run();
    clock_t end = clock();
    double readDuration = duration(start, end);
    timeLog += readDuration;

    std::cout << "Run Complete\n"
    "Copy Duration: " << readDuration << "\n"
    << std::endl;

}

void Timer::recordResults(int numRuns) {
    auto avgTime = [this](int x){return timeLog /(double)x;};
    std::cout << "Number Runs: " << numRuns << "\n"
    "Average Copy time: " << avgTime(numRuns) << "\n"
    << std::endl;
}
