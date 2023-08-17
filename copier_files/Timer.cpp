
#include "Timer.h"

using std::chrono::system_clock;

Timer::Timer(shared_ptr<Writer> thewriter, shared_ptr<Reader> thereader) : thewriter{thewriter}, thereader{thereader}{
    // CPUTimeLog = 0;
    // realTimeLog = 0;
}

void Timer::run() {

    thereader->run();
}

void Timer::runTimed() {
    auto duration = [](clock_t start, clock_t end){return (double)(end - start) / (double)CLOCKS_PER_SEC;};

    std::chrono::time_point<system_clock> startR = system_clock::now();
    clock_t start = clock();
    thereader->run();
    std::chrono::time_point<system_clock> endR = system_clock::now();
    clock_t end = clock();

    //Calculate real time using
    std::chrono::duration<double> timeSeconds = (endR - startR);
    double realTime = timeSeconds.count();

    //Calculate CPU time
    double CPUTime = duration(start, end);
    CPUTimeLog += realTime;

    this->print(realTime, CPUTime);

}

void Timer::print(double real, double CPUTime){
    auto lineFormat = [](std::string s, double v){std::cout << s << std::fixed 
    << std::setprecision(PRECISION) << std::setw(SPACING - s.length()) << v <<"\n";};

    std::cout << "Results" << "\n-------\n";
    lineFormat("Total Real Time: ", real);
    lineFormat("Total CPU Time: ", CPUTime);

}
void Timer::recordResults(int numRuns) {
    auto avgTime = [this](int x){return CPUTimeLog /(double)x;};
    std::cout << "Number Runs: " << numRuns << "\n"
    "Average Copy time: " << avgTime(numRuns) << "\n"
    << std::endl;
}
