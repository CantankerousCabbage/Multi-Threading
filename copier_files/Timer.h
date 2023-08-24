
#include "Reader.h"
#include "Writer.h"
#include <cstdlib>
#include <time.h>
#include <memory>
#include <vector>
#include <chrono>
#include <iomanip>

using std::shared_ptr;
using std::unique_ptr;

#ifndef TIMER
#define TIMER

#define SPACING 40
#define PRECISION 8

class Timer {

    public:
        /* 
        * Creates timer from which we'll time and record our functions
        */
        Timer(shared_ptr<Writer> thewriter, shared_ptr<Reader> thereader);
        /* 
        * Manages timed run
        */
        void runTimed();
        /* 
        * Stores aggregate data on multiple runs
        */
        void recordResults();
        /* 
        * Prints results
        */
        void print(int numRuns=0);
        
    private:

        double realTimeLog;
        double CPUTimeLog;

        double realTime;
        double CPUTime;
        shared_ptr<Writer> thewriter;
        shared_ptr<Reader> thereader;
        unique_ptr<std::vector<double>> archive;
};

#endif
