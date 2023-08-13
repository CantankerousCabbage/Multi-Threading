#include "Timer.h"

// timer::timer(std::string& input, std::string& output, int& numthreads, reader* read, writer* write) 
// : input{input}, output{output}, numThreads{numThreads}, read{read}, write{write} {}
pthread_mutex_t Timer::timeLock; 

Timer::Timer(){};
Timer::Timer( shared_ptr<int>  numRuns) : numRuns{numRuns}{}
Timer::~Timer(){};

void Timer::init(){
     pthread_mutex_init(&timeLock, NULL);
 }
void Timer::start(){
    this->initial = clock();
}

void Timer::end() {
    clock_t end = clock();
    this->total = (double)(end - initial) / (double)CLOCKS_PER_SEC;
}

void Timer::reset() {
        this->writeLockOne = 0;
        this->writeLockTwo = 0;
        this->readLockOne = 0;
        this->readLockTwo = 0;
        
        this->writeCondOne = 0;
        this->writeCondTwo = 0;
        this->readCondOne = 0;
        this->readCondTwo = 0;      
}
       
    
void Timer::updateTime(TimeLog* readLog, shared_ptr<Timer> timer){
    pthread_mutex_lock(&timeLock);
        timer->readLockOne += readLog->lockOne;
        timer->readLockTwo += readLog->lockTwo;
        timer->readCondOne += readLog->condOne;
        timer->readCondTwo += readLog->condTwo;
    pthread_mutex_unlock(&timeLock);
    
}

// void Timer::updateWTime(WriteLog* writeLog, shared_ptr<Timer> timer){
//     pthread_mutex_lock(&timeLock);
//         timer->readLockOne += writeLog->lockOne;
//         timer->readLockTwo += writeLog->lockTwo;
//         timer->readCondOne += writeLog->condOne;
//         timer->readCondTwo += writeLog->condTwo;
//     pthread_mutex_unlock(&timeLock);
    
// }

// class TimeLog{
//     public:
//         clock_t  lockOne;
//         clock_t  lockTwo;
//         clock_t  lockThree;
//         clock_t  condOne;
//         clock_t  condTwo;
// };

// void Timer::updateTime(int item, clock_t time){
//     double timeSec = (double)time / (double)CLOCKS_PER_SEC; 
//     if(item == LOCKONE){
//         this->lockOne += timeSec;
//     } else if(item == LOCKTWO) {
//         this->lockTwo += timeSec;
//     } else if(item == LOCKTHREE) {
//         this->lockThree += timeSec;
//     } else if(item == CONDONE) {
//         this->condOne += timeSec;
//     } else if(item == CONDTWO) {
//         this->condTwo += timeSec;
//     }
// }

