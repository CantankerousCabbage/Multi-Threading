
#include "Timer.h"

Timer::Timer(shared_ptr<Writer> thewriter, shared_ptr<Reader> thereader) : thewriter{thewriter}, thereader{thereader}{}

void Timer::run() {

    std::cout << "Timer" << std::endl;
    thereader->run();
    std::cout << "R Complete" << std::endl;
    // thewriter->run();

    std::cout << "Complete" << std::endl;
}

void Timer::runTimed() {
    auto duration = [](clock_t start, clock_t end){return (start - end) / CLOCKS_PER_SEC;};

    clock_t start = clock();
    thereader->run();
    clock_t end = clock();

    auto readDuration = duration(start, end);

    // start = clock();
    // thewriter->run();
    // end = clock();

    // auto writeDuration = duration(start, end);

    std::cout << "Complete\n"
    "Duration: " << readDuration << "\n"
    << std::endl;

    // std::cout << "Complete\n"
    // "Read Duration: " << readDuration << "\n"
    // "Write Duration: " << writeDuration << "\n"
    // << std::endl;
    
}

