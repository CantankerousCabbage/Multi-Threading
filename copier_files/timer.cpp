#include "timer.h"

timer::timer(writer& thewriter, reader& thereader, const std::string& inFile, const char& t) 
: thewriter{thewriter}, thereader{thereader}, inFile{inFile} {}

void timer::run() {

    auto duration = [](clock_t start, clock_t end){return (start - end) / CLOCKS_PER_SEC;};

    clock_t start = clock();
    thereader.run();
    clock_t end = clock();

    auto readDuration = duration(start, end);

    start = clock();
    thewriter.run();
    end = clock();

    auto writeDuration = duration(start, end);


    //TODO record data
}