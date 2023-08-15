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
#define CONFIG_ADDITIONAL 6
#define DEFAULT 1
#define MAX_THREADS 250
#define MIN_THREADS 4
#define TIMED "-t"

//Thread data object arrays.
Reader** reader;
Writer** writer;  

void cleanup();
int cmdError();

/*
* Generic functions to accomodate reader and writer objects as follows: 
* 1. Generating thread data objects
* 2. Creating threads
* 3. Joining threads.
*/
template<typename T>
void  genObjects(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
        objArray[i] = new T(i);
    }
}

template<typename T>
void  runObjects(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
        std::cout << "Run ID:" << i << std::endl;
        objArray[i]->run();
    }
}

template<typename T>
void logTimes(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
        Timer::updateTime(objArray[i]->tLog);
        
    }
}

template<typename T>
void  joinThreads(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
        std::cout << "Joining ID:" << i << std::endl;
        if(pthread_join(objArray[i]->getThread(), NULL)) {
            std::cout << "Error: unable to join thread" << std::endl;
            exit(-1);
        }
    }
}

/*
* Parses command line, runs validation on input.
*/
bool parseCommandLine(int argc, char** argv, shared_ptr<int> numThreads, std::string& input, 
std::string& output, shared_ptr<bool> timed, shared_ptr<int> numRuns);

int main(int argc, char** argv) {
    //Cleanup method
    atexit(cleanup);
    
    std::string outFile;
    std::string inFile;
    shared_ptr<bool> timed = make_shared<bool>();
    shared_ptr<int> numThreads = make_shared<int>();
    shared_ptr<int> numRuns = make_shared<int>(DEFAULT);
    
    bool success = parseCommandLine(argc, argv, numThreads, inFile, outFile, timed,  numRuns);

    shared_ptr<Timer> timer = (*timed) ? make_shared<Timer>(numRuns) : nullptr;
    int counter = 0;

    if(success) {
        while(counter != *numRuns){
            if(timer) timer->start();
            
            reader = new Reader*[*numThreads];
            writer = new Writer*[*numThreads];
            
            genObjects<Reader>(reader, *numThreads);
            genObjects<Writer>(writer, *numThreads);
            
            Reader::init(inFile, timer);
            Writer::init(outFile, timer);
            
            runObjects<Reader>(reader, *numThreads);
            runObjects<Writer>(writer, *numThreads);

            if(timer){
                logTimes(reader,  *numThreads);
                logTimes(writer,  *numThreads);
            }
            
            joinThreads<Reader>(reader, *numThreads);
            joinThreads<Writer>(writer, *numThreads);

            Reader::cleanUp;
            Writer::cleanUp;

            if(timer){
                timer->end();
                timer->reset();
            }
        }
    } else {
        cmdError();
    }
        
    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, shared_ptr<int> numThreads, string& input, 
string& output, shared_ptr<bool> timed,  shared_ptr<int> numRuns) {

    auto timedCheck = [](string config){return config == TIMED;};
    bool valid = false;
    
    if (argc >= STANDARD_COMMAND && argc <= CONFIG_ADDITIONAL) {
 
        input = string(argv[2]);
        output = string(argv[3]);
        //Try/catch for our integer conversions
        try{  
            *numThreads = (atoi(argv[1])) / 2;
            valid = *numThreads >= MIN_THREADS && *numThreads <= MAX_THREADS;
            if(argc == CONFIG_ADDITIONAL - 1) {
                *timed = timedCheck(string(argv[4]));
                valid = valid && *timed;
            } else if(argc == CONFIG_ADDITIONAL){
                *timed = timedCheck(string(argv[4]));
                *numRuns = atoi(argv[5]);
                valid = (valid && *timed && *numRuns);
            }   
        }catch(std::invalid_argument){
            valid = false;
        }    
    } 

    if(!valid) cmdError();

    return valid;
}

/*
* Cleanup up dynamic memory
*/
void cleanup() {
    delete[] reader;
    delete[] writer;
}

/*
* Error Message
*/
int cmdError() {
    std::cout << 
    "Error, try following input for standard compile:\n"
    "timed: $./mtcopier numthreads infile outfile -t\n" 
    "OR"
    "timed: $./mtcopier numthreads infile outfile -t x\n"
    "x - number of times to run program"
    "untimed: $./mtcopier numthreads infile outfile\n" 
    << MIN_THREADS << " <= Thread count <= " << MAX_THREADS << "\n" 
    << std::endl;

    return 0;
}