#include "Timer.h"

std::string Timer::input;
std::string Timer::output;
int* Timer::numThreads;
Reader* Timer::read;
Writer* Timer::write;

// timer::timer(std::string& input, std::string& output, int& numthreads, reader* read, writer* write) 
// : input{input}, output{output}, numThreads{numThreads}, read{read}, write{write} {}

void Timer::initTimer(std::string& input, std::string& output, int* numThreads, Reader* reader, Writer* writer){
    Timer::input = input;
    Timer::output = output;
    Timer::numThreads = numThreads;
    Timer::read =  reader;
    Timer::write = write;
}
void Timer::run() {

    
    //We want to assign half the threads to read and write respectively so theoretically they keep pace.
    // int x = *numThreads;
     std::cout << *numThreads << std::endl;
    auto threadSplit = [](int x){ return (x / 2) - 1;};
    int realThreads = threadSplit(*numThreads);

    read->init(input, realThreads, write);
    write->init(output, realThreads);

    int arg;
    
    pthread_t readManager;
    // pthread_t writeManager;

    // auto readManagerFunction = [](reader* read){return read->run(NULL);};
    
    pthread_create(&readManager, NULL, readManagerFunction, &arg);
    // pthread_create(&writeManager, NULL, writeManagerFunction, &arg);
    
    pthread_join(readManager, NULL);
    // pthread_join(writeManager, NULL);

    std::cout << "Complete" << std::endl;
}

void* Timer::readManagerFunction(void*) {
    read->run();

    return NULL;
}

void* Timer::writeManagerFunction(void*) {
    write->run();

    return NULL;
}

void Timer::runTimed() {
    auto duration = [](clock_t start, clock_t end){return (start - end) / CLOCKS_PER_SEC;};

    clock_t start = clock();
    read->run();
    clock_t end = clock();

    auto readDuration = duration(start, end);

    start = clock();
    write->run();
    end = clock();

    auto writeDuration = duration(start, end);

    std::cout << "Complete\n"
    "Read Duration: " << readDuration << "\n"
    "Write Duration: " << writeDuration << "\n"
    << std::endl;
    
}

