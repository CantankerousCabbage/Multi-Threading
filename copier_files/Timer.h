
#include "Reader.h"
#include "Writer.h"
#include <cstdlib>
#include <time.h>
#include <memory>

using std::shared_ptr;

#ifndef TIMER
#define TIMER

class Timer {

    public:
        /* Creates timer from which we'll time and record our functions
        * 
        */
        Timer(shared_ptr<Writer> thewriter, shared_ptr<Reader> thereader);

        void run();
        void runTimed();
    private:

        // const std::string& inFile;
        shared_ptr<Writer> thewriter;
        shared_ptr<Reader> thereader;
};

#endif
