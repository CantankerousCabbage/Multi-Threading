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
    pthread_create(&writeThread, NULL, &runner, this);  
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
    
    if(timer) tLog->startLockTimer(); 
      
    pthread_mutex_lock(&queueLock);

    if(timer) tLog->startWaitTimer();
    //While more items incoming read but queue empty wait
    while(!queuedComplete && (queue.size() == 0)){
        Writer::dequeueWait++;
        pthread_cond_wait(&queueCond, &queueLock);
        Writer::dequeueWait--;
    } 
    if(timer) tLog->condOne = tLog->endWaitTimer();

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
    if(timer) tLog->lockOne = tLog->endLockTimer();

    return true; 
}

void Writer::append(const std::string& line, Reader* reader) {
    
    if(timer)  reader->tLog->startLockTimer();

    pthread_mutex_lock(&queueLock);

    if(timer) reader->tLog->startWaitTimer();
    //If buffer full reached or out of order then wait
    while(reader->getReadID() != Reader::queueCounter || queue.size() == BUFFER){
        pthread_cond_wait(&Reader::appendCond, &queueLock);        
    }  
    if(timer) reader->tLog->condOne = reader->tLog->endWaitTimer();

    if(reader->getReadID() == Reader::queueCounter) queue.push_back(line);
    
    //Set queuedComplete when last read appened to queue
    std::cout << "readComplete: " << Reader::readComplete << std::endl;
    std::cout << Reader::readCounter << " vs " << Reader::queueCounter << std::endl;
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

    if(timer) reader->tLog->lockTwo = reader->tLog->endLockTimer();
       
}

void Writer::writeData(){
    
    if(timer) this->tLog->startLockTimer();

     pthread_mutex_lock(&writeLock);
    //Queue to manage writing in correct order
    
    if(timer) this->tLog->startWaitTimer();

    while(!writeComplete && this->writeID != writeCount){
        Writer::writeWait++;
        pthread_cond_wait(&writeCond, &writeLock); 
        Writer::writeWait--;   
    }  
    if(timer) this->tLog->condTwo = this->tLog->endWaitTimer(); 
    
    if(!writeComplete){
        //Prepend newline after first write.
        if(this->writeID != FIRST) out << "\n";
        out << this->writeLine;
        std::cout << "queuedComplete:" << queuedComplete << std::endl;
        std::cout << "WriteCount" << this->writeID << std::endl;
        std::cout << "readCount" << Reader::readCounter << std::endl;
        //If all items remove from buffer set complete
        if(queuedComplete && writeCount == Reader::readCounter){
            std::cout << "COMPLETE" << Writer::dequeueWait << std::endl;
            writeComplete = true;
            pthread_cond_signal(&writeCond);
        } else {
          writeCount++; 
          pthread_cond_broadcast(&writeCond); 
        }
    } else {
         pthread_cond_signal(&writeCond);
    }
    std::cout << "Wait D count" << Writer::dequeueWait << std::endl;
    std::cout << "Wait W count" << Writer::writeWait << std::endl;    
    pthread_mutex_unlock(&writeLock);
    
    if(timer) this->tLog->lockTwo = this->tLog->endLockTimer();
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



