#include "timer.h"

timer::timer(writer& thewriter, reader& thereader) : thewriter{thewriter}, thereader{thereader}{}

void timer::run() {

    thereader.run();
    thewriter.run();

    std::cout << "Complete" << std::endl;
}

void timer::runTimed() {
    auto duration = [](clock_t start, clock_t end){return (start - end) / CLOCKS_PER_SEC;};

    clock_t start = clock();
    thereader.run();
    clock_t end = clock();

    auto readDuration = duration(start, end);

    start = clock();
    thewriter.run();
    end = clock();

    auto writeDuration = duration(start, end);

    std::cout << "Complete\n"
    "Read Duration: " << readDuration << "\n"
    "Write Duration: " << writeDuration << "\n"
    << std::endl;
    
}

