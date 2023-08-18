#include "Timer.h"
#include <iomanip>

// timer::timer(std::string& input, std::string& output, int& numthreads, reader* read, writer* write) 
// : input{input}, output{output}, numThreads{numThreads}, read{read}, write{write} {}
pthread_mutex_t Timer::timeLock; 

Timer::Timer(){};
Timer::Timer( shared_ptr<int>  numRuns) : numRuns{numRuns}{
    this->archive = std::make_unique<std::vector<unique_ptr<archiveEntry>>>();
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
        this->writeCondTwo = 0;
        this->readCondOne = 0;           
}

void Timer::updateReadTime(TimeLog* readLog, shared_ptr<Timer> timer){
    // auto getAvg = [](timeSum time){return time.totalTime / time.entries;}; 
    pthread_mutex_lock(&Timer::timeLock);
        timer->readLockOne = timer->readLockOne + readLog->lockOne.totalTime;
        timer->readLockTwo = timer->readLockTwo + readLog->lockTwo.totalTime;
        timer->readCondOne = timer->readCondOne + readLog->condOne.totalTime;  
    pthread_mutex_unlock(&Timer::timeLock);
    
}

void Timer::updateWriteTime(TimeLog* writeLog, shared_ptr<Timer> timer){
    // auto getAvg = [](timeSum time){return ((time.entries == 0) ? 0 : time.totalTime / time.entries);}; 
    
    pthread_mutex_lock(&Timer::timeLock);
        timer->writeLockOne = timer->writeLockOne + writeLog->lockOne.totalTime;
        timer->writeLockTwo = timer->writeLockTwo + writeLog->lockTwo.totalTime;
        timer->writeCondOne = timer->writeCondOne + writeLog->condOne.totalTime;
        timer->writeCondTwo = timer->writeCondTwo + writeLog->condTwo.totalTime;
    pthread_mutex_unlock(&Timer::timeLock);   
}

double Timer::getAverage(timeSum time){
    return time.totalTime / time.entries;
}

void Timer::printResults(shared_ptr<int> numthreads){
    //Format lambda 
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
    lineFormat("Waiting Write: " , this->writeCondTwo);
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
    archive->back()->writeCondTwo = this->writeCondTwo;
    archive->back()->total = this->total;
    archive->back()->totalCPU =this->totalCPU;

}

void Timer::printAggregate(shared_ptr<int> numThreads,archiveEntry* totals){
    auto lineFormat = [](std::string s, double v){std::cout << s << std::fixed 
    << std::setprecision(PRECISION) << std::setw(SPACING - s.length()) << v <<"\n";};

    std::cout << "Aggregate Results" << "\n-----------------\n";
    std::cout << "#" << (*numRuns) << " runs\n";
    std::cout << "#" << ((*numThreads)*2) << " threads\n";
    lineFormat("Blocked Read Lock: ", totals->readLockOne);
    lineFormat("Blocked Append Lock: ", totals->readLockTwo);
    lineFormat("Blocked Pop Lock: ", totals->writeLockOne);
    lineFormat("Blocked Write Lock: ", totals->writeLockTwo);
    lineFormat("Waiting Append: ", totals->readCondOne);
    lineFormat("Waiting Pop: ", totals->writeCondOne);
    lineFormat("Waiting Write: " , totals->writeCondTwo);
    std::cout << "\n";
    lineFormat("Total Real Time: ", totals->total);
    lineFormat("Total CPU Time: ", totals->totalCPU);
    std::cout << std::endl;
}

void Timer::outputResults( shared_ptr<int> numThreads) {
    archiveEntry* totals = new archiveEntry;

    //Note: At this point i realised disadvantages of structs vs arrays.
    for (int i = 0; i < *numRuns; i++){
        const archiveEntry&  entry = *(*archive)[i];

        totals->readLockOne += entry.readLockOne;
        totals->readLockTwo += entry.readLockTwo;
        totals->readCondOne += entry.readCondOne;
        totals->writeLockOne += entry.writeLockOne;
        totals->writeLockTwo += entry.writeLockTwo;
        totals->writeCondOne += entry.writeCondOne;
        totals->writeCondTwo += entry.writeCondTwo;
        totals->total += entry.total;
        totals->totalCPU +=entry.totalCPU;
    }
    totals->readLockOne = totals->readLockOne / *numRuns; 
    totals->readLockTwo = totals->readLockTwo / *numRuns; 
    totals->readCondOne = totals->readCondOne / *numRuns; 
    totals->writeLockOne = totals->writeLockOne / *numRuns; 
    totals->writeLockTwo = totals->writeLockTwo / *numRuns; 
    totals->writeCondOne = totals->writeCondOne / *numRuns; 
    totals->writeCondTwo = totals->writeCondTwo / *numRuns; 
    totals->total = totals->total / *numRuns; 
    totals->totalCPU = totals->totalCPU / *numRuns; 

    printAggregate(numThreads, totals);

    delete totals;
}

void Timer::cleanUp(){
    pthread_mutex_destroy(&Timer::timeLock);
}


