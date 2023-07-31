/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Timer.h"
#include <cstdlib>
#include <memory>
#include <iostream>

using std::string;
using std::shared_ptr;
using std::make_shared;

#define STANDARD_COMMAND 4
#define CONFIG_ADDITIONAL 5
#define MAX_THREADS 250
#define MIN_THREADS 4
#define TIMED "-t"



Reader* reader;
Writer* writer;  

void cleanup();
int cmdError();

template<typename T>
void  genObjects(T* objArray) {
    for(unsigned i = 0; i < objArray->size(); i++) {
        objArray[i] = new T(i);
    }
}

template<typename T>
void  runObjects(T* objArray) {
    for(unsigned i = 0; i < objArray->size(); i++) {
        objArray[i].run();
    }
}

//Todo
template<typename T>
void  joinThreads(T* objArray) {
    for(unsigned i = 0; i < objArray->size(); i++) {
        if(pthread_join(objArray[i]->getThread(), NULL)) {
            std::cout << "Error: unable to join thread" << std::endl;
            exit(-1);
        }
    }
}

bool parseCommandLine(int argc, char** argv, shared_ptr<int> numThreads, std::string& input, 
std::string& output, shared_ptr<bool> timed);

int main(int argc, char** argv) {
    atexit(cleanup);
    /*
     * 1.   Check command line arguments
     * 
     * 2.   Process command line arguments
     * 
     * 3.   Initiliaze the reader and writer classes
     * 
     * 4.   Initialize the running of each thread. Note you just call run() on each
     *      object here, you'll call pthread_create itself in the run function.
     * 
     * 5.   Have loop here waiting for the threads to bomplete. Please see
     *      section on avoiding busy waiting on the assignment specification to see
     *      what need to be done here
     * 6.   Load the file and copy it to the destination 
     */
    
    std::string outFile;
    std::string inFile;
    shared_ptr<bool> timed = make_shared<bool>();
    shared_ptr<int> numThreads = make_shared<int>();

    bool success = parseCommandLine(argc, argv, numThreads, inFile, outFile, timed);

    if(success) {
        
        reader = new Reader[(*numThreads / 2)];
        writer = new Writer[(*numThreads / 2)];
        genObjects<Reader>(reader);
        genObjects<Writer>(writer);
        Reader::init(inFile);
        Writer::init(outFile);
        runObjects<Reader>(reader);
        runObjects<Writer>(writer);
        joinThreads<Reader>(reader);
        joinThreads<Writer>(writer);

    } else {
        cmdError();
    }
        
    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, shared_ptr<int> numThreads, string& input, 
string& output, shared_ptr<bool> timed){

     shared_ptr<bool> timed;
    shared_ptr<int> numThreads; 
    //c-string to int conversion
    *numThreads = atoi(argv[1]);
    input = string(argv[2]);
    output = string(argv[3]);

    bool valid = *numThreads >= MIN_THREADS && *numThreads <= MAX_THREADS;

    if(argc > STANDARD_COMMAND) {
        *timed = (argc == CONFIG_ADDITIONAL && string(argv[4]) == TIMED) ? true : cmdError();
        valid = valid && *timed;
    }

    return valid;
}

void cleanup() {
    delete[] reader;
    delete[] writer;
}

int cmdError() {
    std::cout << 
    "Error, try following input for standard compile:\n"
    "timed: $./mtcopier numthreads infile outfile -t\n" 
    "untimed: $./mtcopier numthreads infile outfile\n" 
    << MIN_THREADS << " <= Thread count <= " << MAX_THREADS << "\n" 
    << std::endl;

    return 0;
}