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
#define MAX_THREADS 100
#define MIN_THREADS 25
#define TIMED "-t"

//Thread data object arrays.
Reader** reader;
Writer** writer;  
shared_ptr<int> numThreads;
shared_ptr<bool> fileTest;

void cleanup();
int cmdError();

/*
* Generic functions to accomodate reader and writer objects as follows: 
* 1. Generating thread data objects
* 2. Creating threads
* 3. Reset reader/writer instance for multi recorded runs
* 4. Joining threads.
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
* Parses command line
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
    
    //Parse command line
    bool success = parseCommandLine(argc, argv, numThreads, inFile, outFile, timed,  numRuns);
    
    shared_ptr<Timer> timer = make_shared<Timer>(numRuns, timed);

    int counter = 0;
    if(success) {  
            
            //Validate file I/O
            fileTest = make_shared<bool>(false);
            success = success && Reader::init(inFile, timer, fileTest); 
            if(success) success = success && Writer::init(outFile, timer);

            if(success){
                reader = new Reader*[*numThreads];
                writer = new Writer*[*numThreads];
                
                Timer::init();
                
                //Reader and writer instances to encapsulate data/timers.
                genObjects<Reader>(reader, *numThreads);
                genObjects<Writer>(writer, *numThreads);
            }
           
           //Loop to manage multiple runs
            while(success && counter != *numRuns){
                timer->start();
                //Run threads and then join threads
                runObjects<Reader>(reader, *numThreads);
                runObjects<Writer>(writer, *numThreads);
                
                joinThreads<Reader>(reader, *numThreads);
                joinThreads<Writer>(writer, *numThreads);

                //Cleanup I/O
                Reader::close();
                Writer::close();

                //Log times for individual threads
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
                        //Print timer results and archive them if multiple runs 
                        timer->printResults(numThreads);
                        if(*numRuns > DEFAULT) timer->archiveRun();
                    counter++;   
                    if(counter != *numRuns && *numRuns > DEFAULT){
                        //Reset all timer object on multiple runs
                        timer->reset();
                        success = Reader::reset() && Writer::reset();
                        resetObjects<Reader>(reader, *numThreads);
                        resetObjects<Writer>(writer, *numThreads);

                    } else if (*numRuns > DEFAULT && counter == *numRuns){
                        timer->outputResults(numThreads);
                    }
                }             
            }
            //Cleanup all mutex and conditions
            Reader::cleanUp;
            Writer::cleanUp;
            Timer::cleanUp;
            
            if(!success){
                cmdError();
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
    return valid;
}


/*
* Cleanup up dynamic memory
*/
void cleanup() {
   //Make sure array elements initialised before attempting to delete.
   if(fileTest){
        for(int i = 0; i < *numThreads; i++){
        delete reader[i];
        }
        for(int i = 0; i < *numThreads; i++){
            delete writer[i];
        }
   }
    delete[] reader;
    delete[] writer;
}



/*
* Error Message
*/
int cmdError() {
    std::cout << 
    "Error, try following input to run:\n"
    "timed: $./mtcopier numthreads infile outfile -t\n" 
    "OR"
    "timed: $./mtcopier numthreads infile outfile -t x\n"
    "x - number of times to run program"
    "untimed: $./mtcopier numthreads infile outfile\n" 
    << MIN_THREADS << " <= Thread count <= " << MAX_THREADS << "\n\n" 
    "Please check infile exists.\n" 
    << std::endl;

    return 0;
}

