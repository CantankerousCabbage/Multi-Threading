
#ifndef TIMER
#define TIMER

#include <pthread.h>
#include <cstdlib>
#include <time.h>
#include <memory>
#include <vector>

using std::shared_ptr;

#define LOCKONE 1
#define LOCKTWO 2
#define LOCKTHREE 3
#define CONDONE 4
#define CONDTWO 5


class Reader;
class Writer;

class TimeLog {
    public:
        clock_t tmpLock;
        clock_t tmpWait;

        void startLockTimer(){tmpLock = clock();}
        void startWaitTimer(){tmpLock = clock();}
        clock_t endLockTimer(){ return (clock() - tmpLock);}
        clock_t endWaitTimer(){ return (clock() - tmpWait);}

        friend class Timer;
        friend class Reader;
        friend class Writer;
        protected:
            clock_t  lockOne;
            clock_t  lockTwo;
            clock_t  condOne;
            clock_t  condTwo;
};

// class WriteLog : TimeLog{
//     friend class Timer;
//     friend class Writer;
//     protected:

//         clock_t  lockOne;
//         clock_t  lockTwo;
//         clock_t  condOne;
//         clock_t  condTwo;


// };

// class ReadLog : TimeLog{
//     friend class Timer;
//     friend class Reader;
//     friend class Writer;
//     protected:
//         clock_t  lockOne;
//         clock_t  lockTwo;
//         clock_t  condOne;
//         clock_t  condTwo;
// };

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
