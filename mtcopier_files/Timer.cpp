#include "Timer.h"
#include <iomanip>

pthread_mutex_t Timer::timeLock; 
   

    TimeLog::~TimeLog(){}
    TimeLog::TimeLog(){
        this->lockStart = high_resolution_clock::now();
        this->waitStart = high_resolution_clock::now();
        this->lockOne = 0;
        this->IO = 0;
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
        this->IO = 0;
        this->condOne = 0;
    }

    double TimeLog::getDiff(std::chrono::time_point<high_resolution_clock>& start){
        std::chrono::duration<double> timeSeconds = (high_resolution_clock::now() - start);
        return (double)timeSeconds.count(); 
    }



Timer::Timer(){};
Timer::Timer( shared_ptr<int>  numRuns, shared_ptr<bool> timed) : numRuns{numRuns}, timed{timed} {
    this->archive = std::make_unique<std::vector<unique_ptr<archiveEntry>>>();
        this->writeLockOne = 0;
        this->writeIO = 0;
        this->readLockOne = 0;
        this->readIO = 0;
        this->writeCondOne = 0;
        this->readCondOne = 0;
}

Timer::~Timer(){};

void Timer::init(){
     pthread_mutex_init(&timeLock, NULL);
 }
void Timer::start(){
    //Initialise real and CPU times
    this->initial = system_clock::now();
    this->initialCPU = clock();
}

void Timer::end() {
    //Calculating total time using chrono and CPU using clock.
    auto duration = [](clock_t start){return (double)(clock() - start) / (double)CLOCKS_PER_SEC;};
    std::chrono::duration<double> timeSeconds = (system_clock::now() - initial);
    this->total = timeSeconds.count();
    this->totalCPU = duration(this->initialCPU);
}

void Timer::reset() {
        this->writeLockOne = 0;
        this->writeIO = 0;
        this->readLockOne = 0;
        this->readIO = 0;
        this->writeCondOne = 0;
        this->readCondOne = 0;           
}

void Timer::updateReadTime(shared_ptr<TimeLog> readLog, shared_ptr<Timer> timer){
    
    //Record thread specific data to central repository at end of run. Lock to prevent data race. 
    pthread_mutex_lock(&Timer::timeLock);
        timer->readLockOne = timer->readLockOne + readLog->lockOne;
        timer->readIO = timer->readIO + readLog->IO;
        timer->readCondOne = timer->readCondOne + readLog->condOne;  
    pthread_mutex_unlock(&Timer::timeLock);
    
}

void Timer::updateWriteTime(shared_ptr<TimeLog> writeLog, shared_ptr<Timer> timer){
    
    //Record thread specific data to central repository at end of run. Lock to prevent data race.
    pthread_mutex_lock(&Timer::timeLock);
        timer->writeLockOne = timer->writeLockOne + writeLog->lockOne;
        timer->writeIO = timer->writeIO + writeLog->IO;
        timer->writeCondOne = timer->writeCondOne + writeLog->condOne;
    pthread_mutex_unlock(&Timer::timeLock);   
}


void Timer::printResults(shared_ptr<int> numThreads){
    // Format lambda for print output
    auto lineFormat = [](std::string s, double v){std::cout << s << std::fixed 
    << std::setprecision(PRECISION) << std::setw(SPACING - s.length()) << v <<"\n";};

    std::cout << "Results" << "\n-------\n";
    std::cout << "Threads: " << (*numThreads) << ", " << ((*numThreads)*2) << " total." << "\n";
    lineFormat("Blocked Push Lock: ", this->readLockOne);
    lineFormat("Read Time: ", this->readIO);
    lineFormat("Blocked Pop Lock: ", this->writeLockOne);
    lineFormat("Write Time: ", this->writeIO);
    lineFormat("Waiting Push: ", this->readCondOne);
    lineFormat("Waiting Pop: ", this->writeCondOne);
    std::cout << "\n";
    lineFormat("Total Real Time: ", this->total);
    lineFormat("Total CPU Time: ", this->totalCPU);
    std::cout << std::endl;
}

void Timer::archiveRun() {
    //Make archive straight into vector. Make unique pointer for auto management.
    archive->emplace_back(std::make_unique<archiveEntry>());

    archive->back()->readLockOne = this->readLockOne;
    archive->back()->readIO = this->readIO;
    archive->back()->readCondOne = this->readCondOne;
    archive->back()->writeLockOne = this->writeLockOne;
    archive->back()->writeIO = this->writeIO;
    archive->back()->writeCondOne = this->writeCondOne;
    archive->back()->total = this->total;
    archive->back()->totalCPU =this->totalCPU;

}

void Timer::printAggregate(shared_ptr<int> numThreads,shared_ptr<archiveEntry> totals){
    // Format lambda for print output
    auto lineFormat = [](std::string s, double v){std::cout << s << std::fixed 
    << std::setprecision(PRECISION) << std::setw(SPACING - s.length()) << v <<"\n";};

    std::cout << "Aggregate Results" << "\n-----------------\n";
    std::cout << "#" << (*numRuns) << " runs\n";
    std::cout << "Threads: " << (*numThreads) << ", " << ((*numThreads)*2) << " total." << "\n";
    lineFormat("Blocked Push Lock: ", totals->readLockOne);
    lineFormat("Read Time: ", totals->readIO);
    lineFormat("Blocked Pop Lock: ", totals->writeLockOne);
    lineFormat("Write Time: ", totals->writeIO);
    lineFormat("Waiting Push: ", totals->readCondOne);
    lineFormat("Waiting Pop: ", totals->writeCondOne);
    std::cout << "\n";
    lineFormat("Total Real Time: ", totals->total);
    lineFormat("Total CPU Time: ", totals->totalCPU);
    std::cout << std::endl;
}

void Timer::outputResults( shared_ptr<int> numThreads) {
    shared_ptr<archiveEntry> totals = make_shared<archiveEntry>();

    //Loop to aggregate data from archive.
    for (int i = 0; i < *numRuns; i++){
        const archiveEntry&  entry = *(*archive)[i];
        totals->readLockOne = totals->readLockOne + entry.readLockOne;
        totals->readIO = totals->readIO + entry.readIO;
        totals->readCondOne = totals->readCondOne + entry.readCondOne;
        totals->writeLockOne = totals->writeLockOne + entry.writeLockOne;
        totals->writeIO = totals->writeIO + entry.writeIO;
        totals->writeCondOne = totals->writeCondOne + entry.writeCondOne;  
        totals->total = totals->total + entry.total;
        totals->totalCPU = totals->totalCPU + entry.totalCPU;
    }

    //Average data per run then pass to printer
    totals->readLockOne = totals->readLockOne / *numRuns; 
    totals->readIO = totals->readIO / *numRuns; 
    totals->readCondOne = totals->readCondOne / *numRuns; 
    totals->writeLockOne = totals->writeLockOne / *numRuns; 
    totals->writeIO = totals->writeIO / *numRuns; 
    totals->writeCondOne = totals->writeCondOne / *numRuns; 
    totals->total = totals->total / *numRuns; 
    totals->totalCPU = totals->totalCPU / *numRuns; 

    printAggregate(numThreads, totals);

}

void Timer::cleanUp(){
    pthread_mutex_destroy(&Timer::timeLock);
}


