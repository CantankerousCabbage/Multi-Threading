/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Timer.h"
#include <cstdlib>
#include <memory>
#include <iostream>


#define STANDARD_COMMAND 4
#define CONFIG_ADDITIONAL 5
#define MAX_THREADS 10000
#define MIN_THREADS 10
#define TIMED "-t"
/**
 * Removed global variables in favour of explicit cleanup called at exit;
 **/
// void cleanup(reader* read, writer* write, bool* timed, int* numThreads);

Reader* reader;
Writer* writer;
Timer* run;    
bool* timed;
int* numThreads;

void cleanup();
int cmdError();

bool parseCommandLine(int argc, char** argv, int* numThreads, std::string& input, 
std::string& output, bool* timed);

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
    // reader* reader;
    // writer* writer;    
    // bool* timed = new bool(false);
    // int* numThreads = new int(0);

    timed = new bool(false);
    numThreads = new int(0);

    std::cout << *numThreads << std::endl;

    std::string input;
    std::string output;

    bool success = parseCommandLine(argc, argv, numThreads, input, output, timed);
    std::cout <<"debug0" << std::endl;
    run->initTimer(input, output, numThreads, reader, writer);
    std::cout << *numThreads << std::endl;
    // std::unique_ptr<timer> run = std::make_unique<timer>(input, output, numThreads, reader, writer);
    std::cout <<"debug7" << std::endl;
    if (success) {
        (*timed) ? run->runTimed() : run->run();
    }
    
    /* load the file and copy it to the destination */

    // cleanup(reader, writer, timed, numThreads);
    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, int* numThreads, std::string& input, 
std::string& output, bool* timed){

    //c-string to int conversion
    *numThreads = atoi(argv[1]);
    input = std::string(argv[2]);
    output = std::string(argv[3]);

    bool valid = *numThreads >= MIN_THREADS && *numThreads <= MAX_THREADS;

    if(argc > STANDARD_COMMAND) {
        *timed = (argc == CONFIG_ADDITIONAL && std::string(argv[4]) == TIMED) ? true : cmdError();
        valid = valid && *timed;
    }

    return valid;
}

// void cleanup(reader* read, writer* write, bool* timed, int* numThreads) {
//     delete read;
//     delete write;
//     delete timed;
//     delete numThreads;
// }

void cleanup() {
    delete reader;
    delete writer;
    delete timed;
    delete numThreads;
}

int cmdError() {
    std::cout << 
    "Error, try following input for standard compile:\n"
    "timed: $./mtcopier numthreads infile outfile -t\n" 
    "untimed: $./mtcopier numthreads infile outfile\n" 
    "10 <= Thread count <= 10000, \n" 
    << std::endl;

    return 0;
}