
#ifndef TIMER
#define TIMER

#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <time.h>
#include <memory>
#include <vector>
#include <chrono>
#include <iomanip>

#define SPACING 40
#define PRECISION 8

using std::shared_ptr;
using std::chrono::system_clock;
using std::chrono::high_resolution_clock;

class Reader;
class Writer;

struct timeSum{
    long double totalTime = 0;
    double entries = 0;
};

class TimeLog{
     public:
        TimeLog(){
            this->lockStart = high_resolution_clock::now();
            this->waitStart = high_resolution_clock::now();
        };
        //Getclock stores clock data in timespec struc parameter for give clockID.
        void startLockTimer(){
            this->lockStart = high_resolution_clock::now();
        }

        void startWaitTimer(){
            didWait = true; 
            this->waitStart = high_resolution_clock::now();
        }

        void endLockTimer(timeSum& lock){ 
            lock.entries++; 
            lock.totalTime += getDiff(lockStart);
        }
        void endWaitTimer(timeSum& wait){
            if((didWait)) {
                wait.entries++; wait.totalTime += getDiff(waitStart);
                didWait = false;
            }    
        }

        double getDiff(std::chrono::time_point<high_resolution_clock>& start){
            std::chrono::duration<double> timeSeconds = (high_resolution_clock::now() - start);
            return (double)timeSeconds.count(); 
        }

        friend class Timer;
        friend class Reader;
        friend class Writer;

        protected:
            bool didWait;
            timeSum lockOne;
            timeSum lockTwo;
            timeSum condOne;
            timeSum condTwo;
        private:
        
           std::chrono::time_point<high_resolution_clock> lockStart;
           std::chrono::time_point<high_resolution_clock> waitStart;          
};

class Timer {
 
    public:
        Timer();
        Timer(shared_ptr<int>  numRuns);
        ~Timer();

        static void init();
        static void cleanUp();
        static pthread_mutex_t timeLock; 

        void updateReadTime(TimeLog* readLog, shared_ptr<Timer> timer);
        void updateWriteTime(TimeLog* writeLog, shared_ptr<Timer> timer);
        void start();
        void end();
        void reset();
        void printResults(shared_ptr<int> numthreads);
        double getAverage(timeSum time);
        
    private:
        std::vector<double> archive;
        std::chrono::time_point<system_clock> initial;
        clock_t initialCPU;

        double total;
        double totalCPU;
        shared_ptr<int> numRuns;

        double writeLockOne;
        double writeLockTwo;
        double readLockOne;
        double readLockTwo;
        
        double writeCondOne;
        double writeCondTwo;
        double readCondOne;
                
};

#include "Writer.h"
#include "Reader.h"

#endif

