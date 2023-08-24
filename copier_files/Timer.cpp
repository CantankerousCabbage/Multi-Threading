
#include "Timer.h"

using std::chrono::system_clock;

Timer::Timer(shared_ptr<Writer> thewriter, shared_ptr<Reader> thereader) : thewriter{thewriter}, thereader{thereader}{
    this->realTimeLog = 0;
    this->CPUTimeLog = 0;
    this->realTime = 0;
    this->CPUTime = 0;

    archive = std::make_unique<std::vector<double>>();
}

void Timer::runTimed() {
    //Lambda for calculating clock() timerr duration
    auto duration = [](clock_t start, clock_t end){return (double)(end - start) / (double)CLOCKS_PER_SEC;};

    std::chrono::time_point<system_clock> startR = system_clock::now();
    clock_t start = clock();
    thereader->run();
    std::chrono::time_point<system_clock> endR = system_clock::now();
    clock_t end = clock();

    //Store real and CPU time at end of run
    std::chrono::duration<double> timeSeconds = (endR - startR);
    this->realTime = timeSeconds.count();
    this->CPUTime = duration(start, end);
    
    this->print();

}

void Timer::print(int numRuns){
    //Lambda to format results
    auto lineFormat = [](std::string s, double v){std::cout << s << std::fixed 
    << std::setprecision(PRECISION) << std::setw(SPACING - s.length()) << v <<"\n";};
    //Prints standard results or aggregate results in passed a parameter.
    if(!numRuns){
        std::cout << "Results" << "\n-------\n";
        lineFormat("Total Real Time: ", realTime);
        lineFormat("Total CPU Time: ", CPUTime);
        std::cout << std::endl;
    } else {
        std::cout << "Aggregate Results" << "\n-------------\n"
        << "#" << numRuns << "total runs\n";
        lineFormat("Total Real Time: ", (realTimeLog / (double)numRuns));
        lineFormat("Total CPU Time: ", (CPUTimeLog / (double)numRuns)); 
        std::cout << std::endl;
    }
   
}

void Timer::recordResults() {
    CPUTimeLog += CPUTime;
    realTimeLog += realTime;
}
