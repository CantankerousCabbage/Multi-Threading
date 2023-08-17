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
int Writer::lineCount;
int Writer::writeCount;
bool Writer::queuedComplete;
bool Writer::writeComplete;

int Writer::dequeueWait;
int Writer::writeWait;

pthread_mutex_t Writer::queueLock;
pthread_mutex_t Writer::writeLock;
pthread_cond_t Writer::queueCond;
pthread_cond_t Writer::writeCond;

shared_ptr<Timer> Writer::timer;
std::ofstream Writer::out;
std::deque<std::string> Writer::queue;

Writer::Writer(){}
Writer::Writer(int ID) : threadID{ID} {
    tLog = (Writer::timer) ? new TimeLog() : nullptr;
}

Writer::~Writer(){
    delete tLog;
}

void Writer::init(const std::string& name, shared_ptr<Timer> timer) {
    
    Writer::timer = timer;
    out.open(name);

    Writer::queuedComplete = false;
    Writer::writeComplete = false;

    Writer::lineCount = INITIAL;
    Writer::writeCount = 1;
    Writer::dequeueWait = INITIAL;
    Writer::writeWait = INITIAL;

    pthread_mutex_init(&queueLock, NULL);
    pthread_mutex_init(&writeLock, NULL);
    pthread_cond_init(&queueCond, NULL);
    pthread_cond_init(&writeCond, NULL);  
}

void Writer::run() {
    if(pthread_create(&writeThread, NULL, &runner, this)){
        std::cout << "Error: unable create write thread" << std::endl;
        exit(-1);
    }
}

void* Writer::runner(void* arg) { 
    
    Writer* writer = (Writer*) arg;

    while (!writeComplete) {
        writer->dequeue();
        writer->writeData();
    } 
    
    return NULL;
}

bool Writer::dequeue(){
    // std::cout << "Timer: 7" << std::endl;
    if(timer) this->tLog->startLockTimer();  
    pthread_mutex_lock(&queueLock);
    // std::cout << "Timer: 8" << std::endl;
    if(timer) this->tLog->endLockTimer(tLog->lockOne);

    // std::cout << "Timer: 9" << std::endl;
    if(timer) tLog->startWaitTimer();
    //While more items incoming read but queue empty wait
    while(!queuedComplete && (queue.size() == 0)){
        Writer::dequeueWait++;
        pthread_cond_wait(&queueCond, &queueLock);
        Writer::dequeueWait--;
    } 
    // std::cout << "Timer: 10" << std::endl;
    if(timer) tLog->endWaitTimer(tLog->condOne);

    //Safeguard agaisnt outstanding threads in queue on completion
    if(queue.size() != 0){
        this->writeLine = queue.front();
        this->writeID = ++lineCount;
        queue.pop_front();
    }
    //If reading finished empty queue else alternate between push/pop
    if(queuedComplete){
        pthread_cond_signal(&queueCond);   
    } else {
        pthread_cond_broadcast(&Reader::appendCond);    
    }

    pthread_mutex_unlock(&queueLock);

    return true; 
}

void Writer::append(const std::string& line, Reader* reader) {
   
    if(timer)  reader->tLog->startLockTimer();
    pthread_mutex_lock(&queueLock);
  
    if(timer) reader->tLog->endLockTimer(reader->tLog->lockTwo);

  
    if(timer) reader->tLog->startWaitTimer();
    //If buffer full reached or out of order then wait
    while(reader->getReadID() != Reader::queueCounter || queue.size() == BUFFER){
        pthread_cond_wait(&Reader::appendCond, &queueLock);        
    }  

    if(timer) reader->tLog->endWaitTimer(reader->tLog->condOne);

    if(reader->getReadID() == Reader::queueCounter) queue.push_back(line);
    
    if(Reader::readComplete && Reader::readCounter == Reader::queueCounter){
        Writer::setFinished();
    } else {
        Reader::queueCounter++;
    }

    //If items remain to queue broadcast append queue(allows correct line to append), else remove outstanding threads.   
    if(!queuedComplete && queue.size() < BUFFER)
    {
        pthread_cond_broadcast(&Reader::appendCond);
    } else {
        pthread_cond_signal(&queueCond);
    }
    pthread_mutex_unlock(&queueLock); 
       
}

void Writer::writeData(){
    
    // std::cout << "Timer: 11" << std::endl;
    if(timer) this->tLog->startLockTimer();
    pthread_mutex_lock(&writeLock);
    // std::cout << "Timer: 12" << std::endl;
    if(timer) this->tLog->endLockTimer(this->tLog->lockTwo);
    //Queue to manage writing in correct order
    
   
    if(timer) this->tLog->startWaitTimer();
    while(!writeComplete && this->writeID != writeCount){
        Writer::writeWait++;
        pthread_cond_wait(&writeCond, &writeLock); 
        Writer::writeWait--;   
    }  
    // std::cout << "Timer: 14" << std::endl;
    if(timer) this->tLog->endWaitTimer(this->tLog->condTwo); 
    
    if(!writeComplete){
        //Prepend newline after first write.
        if(this->writeID != FIRST) out << "\n";
        out << this->writeLine;
       
        //If all items remove from buffer set complete
        if(queuedComplete && writeCount == Reader::readCounter){
            
            writeComplete = true;
            pthread_cond_signal(&writeCond);
        } else {
          writeCount++; 
          pthread_cond_broadcast(&writeCond); 
        }
    } else {
         pthread_cond_signal(&writeCond);
    }
    
    pthread_mutex_unlock(&writeLock);
}

void Writer::cleanUp() {
    pthread_mutex_destroy(&queueLock);
    pthread_mutex_destroy(&writeLock);
    pthread_cond_destroy(&queueCond);
    pthread_cond_destroy(&writeCond);
}

void Writer::setFinished() {
    queuedComplete = true;
}

pthread_t Writer::getThread(){
    return writeThread;
}

int Writer::getID(){
    return this->threadID;
}



