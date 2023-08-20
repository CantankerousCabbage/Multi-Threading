/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Timer.h"
#include <cstdlib>
#include <memory>
#include <iostream>
#include <vector>
#include <chrono>

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::vector;

#define STANDARD_COMMAND 4
#define CONFIG_ADDITIONAL 6
#define DEFAULT 1
#define MAX_THREADS 250
#define MIN_THREADS 2
#define TIMED "-t"

//Thread data object arrays.
Reader** reader;
Writer** writer;  
shared_ptr<int> numThreads;

void cleanup();
int cmdError();

/*
* Generic functions to accomodate reader and writer objects as follows: 
* 1. Generating thread data objects
* 2. Creating threads
* 3. Reset reader/writer instance for multi recorded runs
* 3. Joining threads.
*/
template<typename T>
void  genObjects(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
        objArray[i] = new T();
    }
}

template<typename T>
void  runObjects(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
        objArray[i]->run();
    }
}

template<typename T>
void  resetObjects(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
        objArray[i]->resetInstance();
    }
}

template<typename T>
void  joinThreads(T** objArray, int& length) {
    for(int i = 0; i < length; i++) {
        if(pthread_join(objArray[i]->getThread(), NULL)) {
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
    numThreads = make_shared<int>();
    shared_ptr<int> numRuns = make_shared<int>(DEFAULT);
    
    bool success = parseCommandLine(argc, argv, numThreads, inFile, outFile, timed,  numRuns);
    
    shared_ptr<Timer> timer = make_shared<Timer>(numRuns);

    int counter = 0;
    if(success) {  
            shared_ptr<bool> fileTest = make_shared<bool>(false);
            reader = new Reader*[*numThreads];
            writer = new Writer*[*numThreads];
            
            success = success && Reader::init(inFile, timer, fileTest) && Writer::init(outFile, timer); 
            
            Timer::init();
            
            genObjects<Reader>(reader, *numThreads);
            genObjects<Writer>(writer, *numThreads);
            
            while(success && counter != *numRuns){
                timer->start();
                 
                runObjects<Reader>(reader, *numThreads);
                runObjects<Writer>(writer, *numThreads);
                 
                joinThreads<Reader>(reader, *numThreads);
                // std::cout << "Reader Joined" << std::endl;
                joinThreads<Writer>(writer, *numThreads);
                // std::cout << "Writer Joined" << std::endl;

                Reader::close();
                Writer::close();

                if(*timed){
                    timer->end();
                    for(int i = 0; i < *numThreads; i++) {
                        timer->updateReadTime(reader[i]->tLog, timer);
                    }
                    for(int i = 0; i < *numThreads; i++) {
                        timer->updateWriteTime(writer[i]->tLog, timer);
                    }
                }

                if(*timed){
                        timer->printResults(numThreads);
                        if(*numRuns > DEFAULT) timer->archiveRun();
                    counter++;   
                    if(counter != *numRuns && *numRuns > DEFAULT){
                        
                        
                        timer->reset();
                        success = Reader::reset() && Writer::reset();
                        resetObjects<Reader>(reader, *numThreads);
                        resetObjects<Writer>(writer, *numThreads);

                    } else if (*numRuns > DEFAULT && counter == *numRuns){
                        timer->outputResults(numThreads);
                    }
                }          
                // counter++;    
            }
            Reader::cleanUp;
            Writer::cleanUp;
            Timer::cleanUp;
            
            if(!success) cmdError();
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
            *numThreads = (atoi(argv[1]));
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
    *numThreads = *numThreads / 2;

    if(!valid) cmdError();

    return valid;
}

/*
* Cleanup up dynamic memory
*/
void cleanup() {
    for(int i = 0; i < *numThreads; i++){
        delete reader[i];
    }
    for(int i = 0; i < *numThreads; i++){
        delete writer[i];
    }
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

