
#include "Reader.h"
#include "Writer.h"
#include <cstdlib>
#include <time.h>
#include <memory>
#include <vector>
#include <chrono>
#include <iomanip>

using std::shared_ptr;

#ifndef TIMER
#define TIMER

#define SPACING 40
#define PRECISION 8

class Timer {

    public:
        /* Creates timer from which we'll time and record our functions
        * 
        */
        Timer(shared_ptr<Writer> thewriter, shared_ptr<Reader> thereader);

        void run();
        void runTimed();
        void recordResults(int runs);
        void print(double real, double CPUTime);
    private:

        // const std::string& inFile;
        double realTimeLog;
        double CPUTimeLog;
        shared_ptr<Writer> thewriter;
        shared_ptr<Reader> thereader;
};

#endif
