
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

/**
 * Structure for recording all data from a single run. Recalled to aggregate data on 
 * multiple runs.
 **/
struct archiveEntry{      
    double writeLockOne = 0;
    double writeIO = 0;
    double readLockOne = 0;
    double readIO = 0;
    double writeCondOne = 0;
    double readCondOne = 0;

    double total = 0;
    double totalCPU = 0;  
    
};

/**
 * Instanced class for recording distinct thread timings
 **/
class TimeLog{
     public:
        TimeLog();
        ~TimeLog();
        /**
         * Stores real time when attempting to acquire lock.
         **/
        void startLockTimer();

        /**
         * Stores real time on entering conditional variable
         **/
        void startWaitTimer();

        /**
         * Calculates time since startLockTimer
         **/
        void endLockTimer(double& lock);

        /**
         * Calculate time since endWaitTimer
         **/
        void endWaitTimer();

        /**
         * Reset timelog after data is archived when multiple runs enabled 
         **/
        void reset();

        /**
         * Calculates difference between start and end times
         **/
        double getDiff(std::chrono::time_point<high_resolution_clock>& start);

        friend class Timer;
        friend class Reader;
        friend class Writer;

        protected:
            bool didWait;
            double lockOne;
            double IO;
            double condOne;
        private:
        
           std::chrono::time_point<high_resolution_clock> lockStart;
           std::chrono::time_point<high_resolution_clock> waitStart;          
};

class Timer {
 
    public:
        Timer();
        Timer(shared_ptr<int>  numRuns, shared_ptr<bool> time);
        ~Timer();

        /**
         * Initialises globabl timer variables
         **/
        static void init();

        /**
         *  Destroys timer mutex, that prevents race conditions in archive entry
         **/
        static void cleanUp();
        static pthread_mutex_t timeLock; 

        /**
         *  Stores data from calling read thread into archive struct
         **/
        void updateReadTime(shared_ptr<TimeLog> readLog, shared_ptr<Timer> timer);

         /**
         *  Stores data from calling write thread into archive struct
         **/
        void updateWriteTime(shared_ptr<TimeLog> writeLog, shared_ptr<Timer> timer);

         /**
         *  Starts timer for total program time, CPU and real.
         **/
        void start();

        /**
         *  Ends timer for total program time, CPU and real.
         **/
        void end();

        /**
         *  Resets the aggregate timing results after archive entry for multiple runs
         **/
        void reset();

        /**
         *  Prints individual run results to terminal.
         **/
        void printResults(shared_ptr<int> numthreads);

        /**
         *  Prints aggregate results to terminal. Only on multiple runs.
         **/
        void printAggregate(shared_ptr<int> numthreads, shared_ptr<archiveEntry> totals);

        /**
         *  Stores aggregate run data into archive struct and palces in archive vector.
         **/
        void archiveRun();

        /**
         *  Aggregates multi-run results before invoking printAggregate
         **/
        void outputResults(shared_ptr<int> numThreads);

        friend class Reader;
        friend class Writer;
    protected:
        shared_ptr<bool> timed;
    private:
        unique_ptr<std::vector<unique_ptr<archiveEntry>>> archive;
        std::chrono::time_point<system_clock> initial;
        clock_t initialCPU;

        double total;
        double totalCPU;
        shared_ptr<int> numRuns;

        double writeLockOne;
        double writeIO;
        double readLockOne;
        double readIO;
        double writeCondOne;
        double readCondOne;
                
};

#include "Writer.h"
#include "Reader.h"

#endif

