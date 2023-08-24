/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Writer.h"
#include "Timer.h"
#include "Reader.h"

/**
 * Static variables for managing synchronisation
 **/
string Writer::outName;

bool Writer::finished;
bool Writer::first;
bool Writer::empty;

pthread_mutex_t Writer::queueLock;
pthread_mutex_t Writer::writeLock;
pthread_cond_t Writer::popCond;
pthread_cond_t Writer::writeCond;

shared_ptr<Timer> Writer::timer;
std::ofstream Writer::out;
std::deque<std::string> Writer::queue;

Writer::Writer() {
    tLog = make_shared<TimeLog>();
}

Writer::~Writer(){}

bool Writer::init(const std::string& name, shared_ptr<Timer> timer) {
    
    Writer::timer = timer;
    Writer::outName = name;
    out.open(name);
    bool fileCheck = out.good();
    if (fileCheck) {
        Writer::finished = false;
        Writer::first = true;
        Writer::empty = false;

        pthread_mutex_init(&queueLock, NULL);
        pthread_mutex_init(&writeLock, NULL);
        pthread_cond_init(&popCond, NULL);
        pthread_cond_init(&writeCond, NULL);  
    }
    return fileCheck;
}

void Writer::run() {
    if(pthread_create(&writeThread, NULL, &runner, this)){
        std::cout << "Error: unable create write thread" << std::endl;
        exit(-1);
    }
}

void* Writer::runner(void* arg) { 
    
    Writer* writer = (Writer*) arg;
    writer->execute(); 
    return NULL;
}
void Writer::execute(){

    //Execute until reading finished and queue empty
    while (!empty) {
        //Start Blocked Push Lock timer
        if(this->timer->timed)this->tLog->startLockTimer();  
        pthread_mutex_lock(&queueLock);
        if(this->timer->timed)this->tLog->endLockTimer(tLog->lockOne);

        //Start Waiting Pop timer
        if(this->timer->timed)this->tLog->startWaitTimer();
        //If reading not finished but queue empty then wait
        while(!queue.size() && !finished){
            pthread_cond_wait(&popCond, &queueLock);
        } 
        if(this->timer->timed)this->tLog->endWaitTimer();

        this->dequeue();

        //Write time timer
        if(this->timer->timed)this->tLog->startLockTimer();
        this->writeData();
        if(this->timer->timed)this->tLog->endLockTimer(tLog->IO);

        pthread_mutex_unlock(&queueLock);
        
    } 
}
void Writer::dequeue(){
    //Only pop if elements. If finished and empty flag.
    if(queue.size()){
        this->writeLine = queue.front();
        queue.pop_front();
    } else if(finished) {
        empty = true;
    }

    //Signal pop till buffer flushed otherwise trigger read appends 
    if(queue.size()){
        pthread_cond_signal(&popCond);   
    } else {   
        pthread_cond_signal(&Reader::pushCond);        
    }

}

void Writer::writeData(){
    if(!empty){
        
        if(!Writer::first){
            out << "\n";   
        } 
        if(Writer::first){
            Writer::first = false; 
        } 
        out << this->writeLine;
    }
}

void Writer::append(const std::string& line, Reader* reader) {
   
    queue.push_back(line);
     
}

void Writer::cleanUp() {
    pthread_mutex_destroy(&queueLock);
    pthread_mutex_destroy(&writeLock);
    pthread_cond_destroy(&popCond);
    
}

bool Writer::reset(){
    out.open(outName);
    bool fileCheck = out.good();
    if(fileCheck) {
        Writer::finished = false;
        Writer::first = true;
        Writer::empty = false;
    }
   return fileCheck;
}

void Writer::resetInstance() {
    this->writeLine = "";
    this->tLog->reset();
}

void Writer::setFinished() {
    finished = true;
}

pthread_t Writer::getThread() {
    return writeThread;
}

void Writer::close(){
    Writer::out.close();
}


