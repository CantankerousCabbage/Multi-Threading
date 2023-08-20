
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
using std::unique_ptr;
using std::chrono::system_clock;
using std::chrono::high_resolution_clock;

class Reader;
class Writer;

// struct timeSum{
//     double totalTime = 0;  
// };

struct archiveEntry{      
    double writeLockOne = 0;
    double writeLockTwo = 0;
    double readLockOne = 0;
    double readLockTwo = 0;
    
    double writeCondOne = 0;
    double readCondOne = 0;

    double total = 0;
    double totalCPU = 0;  
    
};

class TimeLog{
     public:
        TimeLog();
        ~TimeLog();
        //Getclock stores clock data in timespec struc parameter for give clockID.
        void startLockTimer();

        void startWaitTimer();

        void endLockTimer(double& lock);
        void endWaitTimer();

        void reset();

        double getDiff(std::chrono::time_point<high_resolution_clock>& start);

        friend class Timer;
        friend class Reader;
        friend class Writer;

        protected:
            bool didWait;
            double lockOne;
            double lockTwo;
            double condOne;
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

        void updateReadTime(shared_ptr<TimeLog> readLog, shared_ptr<Timer> timer);
        void updateWriteTime(shared_ptr<TimeLog> writeLog, shared_ptr<Timer> timer);
        void start();
        void end();
        void reset();
        void printResults(shared_ptr<int> numthreads);
        void printAggregate(shared_ptr<int> numthreads, shared_ptr<archiveEntry> totals);
        void archiveRun();
        void outputResults(shared_ptr<int> numThreads);
        
    private:
        unique_ptr<std::vector<unique_ptr<archiveEntry>>> archive;
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
        double readCondOne;
                
};

#include "Writer.h"
#include "Reader.h"

#endif

