#include "Timer.h"
#include <iomanip>

pthread_mutex_t Timer::timeLock; 
   

    TimeLog::~TimeLog(){}
    TimeLog::TimeLog(){
        this->lockStart = high_resolution_clock::now();
        this->waitStart = high_resolution_clock::now();
        this->lockOne = 0;
        this->lockTwo = 0;
        this->condOne = 0;
        didWait = false;
    };

    //Getclock stores clock data in timespec struc parameter for give clockID.
    void TimeLog::startLockTimer(){
        this->lockStart = high_resolution_clock::now();
    }

    void TimeLog::startWaitTimer(){
        didWait = true; 
        this->waitStart = high_resolution_clock::now();
    }

    void TimeLog::endLockTimer(double& lock){  
        lock += getDiff(lockStart);
    }
    void TimeLog::endWaitTimer(){
        if((didWait)) {
            condOne += getDiff(waitStart);
            didWait = false;
        }    
    }

    void TimeLog::reset() {
        this->lockOne = 0;
        this->lockTwo = 0;
        this->condOne = 0;
    }

    double TimeLog::getDiff(std::chrono::time_point<high_resolution_clock>& start){
        std::chrono::duration<double> timeSeconds = (high_resolution_clock::now() - start);
        return (double)timeSeconds.count(); 
    }



Timer::Timer(){};
Timer::Timer( shared_ptr<int>  numRuns) : numRuns{numRuns}{
    this->archive = std::make_unique<std::vector<unique_ptr<archiveEntry>>>();
        this->writeLockOne = 0;
        this->writeLockTwo = 0;
        this->readLockOne = 0;
        this->readLockTwo = 0;
        this->writeCondOne = 0;
        this->readCondOne = 0;
}

Timer::~Timer(){};

void Timer::init(){
     pthread_mutex_init(&timeLock, NULL);
 }
void Timer::start(){
    this->initial = system_clock::now();
    this->initialCPU = clock();
}

void Timer::end() {
    auto duration = [](clock_t start){return (double)(clock() - start) / (double)CLOCKS_PER_SEC;};
    std::chrono::duration<double> timeSeconds = (system_clock::now() - initial);
    this->total = timeSeconds.count();
    this->totalCPU = duration(this->initialCPU);
}

void Timer::reset() {
        this->writeLockOne = 0;
        this->writeLockTwo = 0;
        this->readLockOne = 0;
        this->readLockTwo = 0;
        this->writeCondOne = 0;
        this->readCondOne = 0;           
}

void Timer::updateReadTime(shared_ptr<TimeLog> readLog, shared_ptr<Timer> timer){
    // auto getAvg = [](timeSum time){return time.totalTime / time.entries;}; 
    pthread_mutex_lock(&Timer::timeLock);
        timer->readLockOne = timer->readLockOne + readLog->lockOne;
        timer->readLockTwo = timer->readLockTwo + readLog->lockTwo;
        timer->readCondOne = timer->readCondOne + readLog->condOne;  
    pthread_mutex_unlock(&Timer::timeLock);
    
}

void Timer::updateWriteTime(shared_ptr<TimeLog> writeLog, shared_ptr<Timer> timer){
    // auto getAvg = [](timeSum time){return ((time.entries == 0) ? 0 : time.totalTime / time.entries);}; 
    
    pthread_mutex_lock(&Timer::timeLock);
        timer->writeLockOne = timer->writeLockOne + writeLog->lockOne;
        timer->writeLockTwo = timer->writeLockTwo + writeLog->lockTwo;
        timer->writeCondOne = timer->writeCondOne + writeLog->condOne;
    pthread_mutex_unlock(&Timer::timeLock);   
}


void Timer::printResults(shared_ptr<int> numthreads){
    // Format lambda 
    auto lineFormat = [](std::string s, double v){std::cout << s << std::fixed 
    << std::setprecision(PRECISION) << std::setw(SPACING - s.length()) << v <<"\n";};

    std::cout << "Results" << "\n-------\n";
    std::cout << "#" << ((*numthreads)*2) << " threads\n";
    lineFormat("Blocked Read Lock: ", this->readLockOne);
    lineFormat("Blocked Append Lock: ", this->readLockTwo);
    lineFormat("Blocked Pop Lock: ", this->writeLockOne);
    lineFormat("Blocked Write Lock: ", this->writeLockTwo);
    lineFormat("Waiting Append: ", this->readCondOne);
    lineFormat("Waiting Pop: ", this->writeCondOne);
    std::cout << "\n";
    lineFormat("Total Real Time: ", this->total);
    lineFormat("Total CPU Time: ", this->totalCPU);
    std::cout << std::endl;
}

void Timer::archiveRun() {
    //Make our archive entry at back of vector then save values.
    archive->emplace_back(std::make_unique<archiveEntry>());

    archive->back()->readLockOne = this->readLockOne;
    archive->back()->readLockTwo = this->readLockTwo;
    archive->back()->readCondOne = this->readCondOne;
    archive->back()->writeLockOne = this->writeLockOne;
    archive->back()->writeLockTwo = this->writeLockTwo;
    archive->back()->writeCondOne = this->writeCondOne;
    archive->back()->total = this->total;
    archive->back()->totalCPU =this->totalCPU;

}

void Timer::printAggregate(shared_ptr<int> numThreads,shared_ptr<archiveEntry> totals){
    auto lineFormat = [](std::string s, double v){std::cout << s << std::fixed 
    << std::setprecision(PRECISION) << std::setw(SPACING - s.length()) << v <<"\n";};
    std::cout << "AT PRINT" << std::endl;
    std::cout << "Aggregate Results" << "\n-----------------\n";
    std::cout << "#" << (*numRuns) << " runs\n";
    std::cout << "#" << ((*numThreads)*2) << " threads\n";
    lineFormat("Blocked Read Lock: ", totals->readLockOne);
    lineFormat("Blocked Append Lock: ", totals->readLockTwo);
    lineFormat("Blocked Pop Lock: ", totals->writeLockOne);
    lineFormat("Blocked Write Lock: ", totals->writeLockTwo);
    lineFormat("Waiting Append: ", totals->readCondOne);
    lineFormat("Waiting Pop: ", totals->writeCondOne);
    std::cout << "\n";
    lineFormat("Total Real Time: ", totals->total);
    lineFormat("Total CPU Time: ", totals->totalCPU);
    std::cout << std::endl;
}

void Timer::outputResults( shared_ptr<int> numThreads) {
    shared_ptr<archiveEntry> totals = make_shared<archiveEntry>();

    for (int i = 0; i < *numRuns; i++){
        const archiveEntry&  entry = *(*archive)[i];
        totals->readLockOne = totals->readLockOne + entry.readLockOne;
        totals->readLockTwo = totals->readLockTwo + entry.readLockTwo;
        totals->readCondOne = totals->readCondOne + entry.readCondOne;
        totals->writeLockOne = totals->writeLockOne + entry.writeLockOne;
        totals->writeLockTwo = totals->writeLockTwo + entry.writeLockTwo;
        totals->writeCondOne = totals->writeCondOne + entry.writeCondOne;  
        totals->total = totals->total + entry.total;
        totals->totalCPU = totals->totalCPU + entry.totalCPU;
    }

    totals->readLockOne = totals->readLockOne / *numRuns; 
    totals->readLockTwo = totals->readLockTwo / *numRuns; 
    totals->readCondOne = totals->readCondOne / *numRuns; 
    totals->writeLockOne = totals->writeLockOne / *numRuns; 
    totals->writeLockTwo = totals->writeLockTwo / *numRuns; 
    totals->writeCondOne = totals->writeCondOne / *numRuns; 
    totals->total = totals->total / *numRuns; 
    totals->totalCPU = totals->totalCPU / *numRuns; 

    printAggregate(numThreads, totals);

}

void Timer::cleanUp(){
    pthread_mutex_destroy(&Timer::timeLock);
}


