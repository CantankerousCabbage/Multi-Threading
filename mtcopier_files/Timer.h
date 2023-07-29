
#ifndef TIMER
#define TIMER

#include "Writer.h"
#include "Reader.h"
#include <cstdlib>
#include <time.h>

class Reader;
class Writer;

class Timer {

    public:
        /* Creates timer from which we'll time and record our functions
        * 
        */
        // timer(std::string& input, std::string& output, int& numthreads, reader* reader, writer* write);
        static void initTimer(std::string& input, std::string& output, int* numthreads, Reader* reader, Writer* writer);
        static void* readManagerFunction(void*);
        static void* writeManagerFunction(void*);

        void run();
        void runTimed();
    // private:

        // const std::string& inFile;
        static std::string input;
        static std::string output;
        static int* numThreads;
        static Reader* read;
        static Writer* write;
};
#endif
