
#ifndef TIMER
#define TIMER

#include <pthread.h>
#include <cstdlib>
#include <time.h>
#include <memory>
#include <vector>


    //    #include <errno.h>
    //    #include <pthread.h>
    //    #include <stdint.h>
    //    #include <stdio.h>
    //    #include <stdlib.h>
    //    #include <string.h>
    //    #include <time.h>
    //    #include <unistd.h>

using std::shared_ptr;

#define NANO 1000000000
#define LOCKONE 1
#define LOCKTWO 2
#define LOCKTHREE 3
#define CONDONE 4
#define CONDTWO 5

//timespec structure for reference
// struct timespec {
//         time_t   tv_sec;        /* seconds */
//         long     tv_nsec;       /* nanoseconds */
// };

class Reader;
class Writer;

class TimeLog {
    public:
        TimeLog(pthread_t thread) : thread{thread} {
            //Get the thread specific clock ID
            if(!pthread_getcpuclockid(thread, &clockID)){
                std::cout << "Failure to obtain thread clock ID\n"
                "Abort program" << std::endl;
                exit(-1);
            }
        };
        //Getclock stores clock data in timespec struc parameter for give clockID.
        void startLockTimer(){clock_gettime(clockID, &tmpLockStart);}
        void startWaitTimer(){clock_gettime(clockID, &tmpWaitStart);}

        void endLockTimer(double lock){lock = getDiff(tmpLockStart);}
        void endWaitTimer(double wait){wait = getDiff(tmpWaitStart);}

        //Covert difference in two timespec struct's to seconds.
        double getDiff(const struct timespec &start){
            struct timespec end;
            clock_gettime(clockID, &end);
            return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / NANO;
        }
        friend class Timer;
        friend class Reader;
        friend class Writer;
        protected:
        //Time to receive or wait for a lock
            double lockOne;
            double lockTwo;
            double condOne;
            double condTwo;
        private:
        //Variables for accessing specific thread CPU-timer
            struct timespec tmpLockStart;
            struct timespec tmpWaitStart;
            pthread_t thread;
            clockid_t clockID;

            
};



class Timer {

    
    public:
        Timer();
        Timer( shared_ptr<int>  numRuns);
        ~Timer();

        static void init();
        static void updateTime(TimeLog* readLog,  shared_ptr<Timer> timer);
        // void Timer::updateWTime(WriteLog* writeLog, shared_ptr<Timer> timer);
        void start();
        void end();
        void reset();

       static pthread_mutex_t timeLock; 
    private:
        std::vector<double> archive;
        clock_t initial;

        double total;
        shared_ptr<int> numRuns;

        double writeLockOne;
        double writeLockTwo;
        double readLockOne;
        double readLockTwo;
        
        double writeCondOne;
        double writeCondTwo;
        double readCondOne;
        double readCondTwo;
        
};

#include "Writer.h"
#include "Reader.h"

#endif
