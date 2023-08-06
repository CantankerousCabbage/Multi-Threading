/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Timer.h"
#include <cstdlib>
#include <memory>
#include <iostream>
#include <vector>

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::vector;

#define STANDARD_COMMAND 4
#define CONFIG_ADDITIONAL 5
#define MAX_THREADS 250
#define MIN_THREADS 4
#define TIMED "-t"



Reader** reader;
Writer** writer;  

void cleanup();
int cmdError();

template<typename T>
void  genObjects(T** objArray, int& length) {
    std::cout << "Size Array: " << length << std::endl;
    for(int i = 0; i < length; i++) {
        std::cout << "Initialised Object ID:" << i << std::endl;
        objArray[i] = new T(i);
        std::cout << "Initialised Object ID:" << objArray[i]->getID() << std::endl;
    }
}

template<typename T>
void  runObjects(T** objArray, int& length) {
    std::cout << "Run Start Loop" << std::endl;
    for(int i = 0; i < length; i++) {
        std::cout << "Running Thread: " << i << std::endl;
        objArray[i]->run();
    }
}

//Todo
template<typename T>
void  joinThreads(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
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
        std::cout << "numThreads: " << *numThreads << std::endl;
        std::cout << "Pointers" << std::endl;
        reader = new Reader*[25];
        
        writer = new Writer*[25];
        std::cout << "Gen" << std::endl;
        genObjects<Reader>(reader, *numThreads);
        genObjects<Writer>(writer, *numThreads);
        std::cout << "Init" << std::endl;
        Reader::init(inFile);
        Writer::init(outFile);
        std::cout << "Run" << std::endl;
        runObjects<Reader>(reader, *numThreads);
        std::cout << "Run Write" << std::endl;
        runObjects<Writer>(writer, *numThreads);
        std::cout << "Join" << std::endl;
        joinThreads<Reader>(reader, *numThreads);
        joinThreads<Writer>(writer, *numThreads);
        std::cout << "Complete" << std::endl;

    } else {
        cmdError();
    }
        
    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, shared_ptr<int> numThreads, string& input, 
string& output, shared_ptr<bool> timed){
 
   bool valid;
    
    if (argc < STANDARD_COMMAND) {
        cmdError(); 
    } else  {   
        //c-string to int conversion 
        *numThreads = (atoi(argv[1])) / 2;
        input = string(argv[2]);
        output = string(argv[3]);
        valid = *numThreads >= MIN_THREADS && *numThreads <= MAX_THREADS;
        
        if(argc > STANDARD_COMMAND) {
            *timed = (argc == CONFIG_ADDITIONAL && string(argv[4]) == TIMED) ? true : cmdError();
            valid = valid && *timed;
        } 
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