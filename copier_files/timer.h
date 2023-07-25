
#include "reader.h"
#include "writer.h"
#include <cstdlib>
#include <time.h>

#ifndef TIMER
#define TIMER

class timer {

    public:
        /* Creates timer from which we'll time and record our functions
        * 
        */
        timer(writer& thewriter, reader& thereader, const std::string& inFile, const char& t);

        void run();
    private:

        const std::string& inFile;
        writer& thewriter;
        reader& thereader;
};
#endif
