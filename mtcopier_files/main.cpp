/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
#include <cstdlib>
#include <memory>


#define STANDARD_COMMAND 4
#define CONFIG_ADDITIONAL 5
#define TIMED "-t"
/**
 * these need to be global variables as you'll need handle cleaning them up in
 *cleanup which will automatically be called on program exit
 **/
reader* readers;
writer* writers;

void cleanup() {
    /**
     * perform any cleanup you need to do here for global pointers
     **/
}

int main(int argc, char** argv) {
    atexit(cleanup);
    /**
     * check command line arguments
     **/
    /**
     * process command line arguments
     **/
    /**
     * initiliaze the reader and writer classes
     **/
    /**
     * initialize the running of each thread. Note you just call run() on each
     * object here, you'll call pthread_create itself in the run function.
     **/

    /**
     *
     * have loop here waiting for the threads to bomplete. Please see
     * section on avoiding busy waiting on the assignment specification to see
     * what need to be done here
      **/
    
    bool* timed;
    int* numThreads;
    std::string input;
    std::string output;

    parseCommandLine(argc, argv, numThreads, input, output, timed);

    
    /* load the file and copy it to the destination */
    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, int* numThreads, std::string& input, 
std::string& output, bool* timed){

    *numThreads = (unsigned)argv[2];
    input = std::string(argv[1]);
    output = std::string(argv[2]);

    if(argc > STANDARD_COMMAND) {
        *timed = (argc == CONFIG_ADDITIONAL && argv[3] == TIMED) ? true : cmdError();
    }
    
}

int cmdError() {
    std::cout << 
    "Error, try following input for standard compile:\n"
    "timed: $./mtcopier numthreads infile outfile -t\n" 
    "untimed: $./mtcopier numthreads infile outfile -t\n" 
    << std::endl;

    return 0;
}