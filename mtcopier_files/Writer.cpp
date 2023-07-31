/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Writer.h"
#include "Timer.h"
#include "Reader.h"

int Writer::lineCount;
int Writer::writeCount;
bool Writer::writeComplete;

pthread_mutex_t* Writer::queueLock;
pthread_mutex_t* Writer::fetchLock;
pthread_mutex_t* Writer::writeLock;

pthread_cond_t* Writer::queueCond;
pthread_cond_t* Writer::writeCond;

std::string Writer::outFile;
std::ofstream Writer::out;
std::deque<std::string> Writer::queue;





Writer::Writer(){}
Writer::Writer(int ID) : threadID{ID} {}
Writer::~Writer(){}

void Writer::init(const std::string& name) {
    
    Writer::outFile = name;
    out.open(outFile);

    Writer::writeComplete = false;
    Writer::lineCount = 0;
    Writer::writeCount = 1;
 
    // pthread_mutex_t* queueLock = new pthread_mutex_t;
    // pthread_mutex_t* fetchLock = new pthread_mutex_t;
    // pthread_mutex_t* writeLock = new pthread_mutex_t;

    // pthread_cond_t* queueCond = new pthread_cond_t;
    // pthread_cond_t* writeCond = new pthread_cond_t;

    pthread_mutex_init(fetchLock, NULL);
    pthread_mutex_init(queueLock, NULL);
    pthread_mutex_init(writeLock, NULL);
    pthread_cond_init(queueCond, NULL);
    pthread_cond_init(writeCond, NULL);  
}

void Writer::run() {

    pthread_create(writeThread, NULL, &runner, this);
    cleanUp();
}

void* Writer::runner(void* arg) { 
    Writer* writer = (Writer*) arg;
    
    while (!writeComplete) {
        writer->fetchData();
        writer->writeData();
    }
    std::cout <<"W3" << std::endl;
    return nullptr; 
}

void Writer::fetchData(){
    //If dequeue unsuccesful wait for signal
    
    pthread_mutex_lock(fetchLock);
    
    if (writeComplete && writeCount == Reader::readCounter) { 
        pthread_mutex_unlock(fetchLock);
        pthread_exit(NULL);
    } else {
        dequeue();
    pthread_mutex_unlock(fetchLock);

    }
}

void Writer::append(const std::string& line) {
      
    pthread_mutex_lock(queueLock);
        queue.push_back(line);
    pthread_cond_signal(queueCond);
    pthread_mutex_unlock(queueLock);   
}

bool Writer::dequeue(){
    //Wait for signal if queue empty.
    pthread_mutex_lock(queueLock);
    while(!queue.size()){
        pthread_cond_wait(queueCond, queueLock);
    } 
        this->writeLine = queue[0];
        this->writeID = ++lineCount;
        queue.pop_front();
    pthread_mutex_unlock(queueLock);
    
    return true; 
}

void Writer::writeData(){
     pthread_mutex_lock(writeLock);

    while(this->writeID != writeCount){
        pthread_cond_wait(writeCond, writeLock);
    }
        out << this->writeLine; 
        writeCount++;    
    pthread_cond_broadcast(writeCond);
    pthread_mutex_unlock(writeLock);

}

void Writer::cleanUp() {
    pthread_mutex_destroy(queueLock);
    pthread_mutex_destroy(fetchLock);
    pthread_mutex_destroy(writeLock);
    pthread_cond_destroy(queueCond);
    pthread_cond_destroy(writeCond);

    // delete queueLock;
    // delete fetchLock;
    // delete writeLock;
    // delete queueCond;
    // delete writeCond;
}

void Writer::setFinal(int* count)  {
    finalCount = count;
}

void Writer::setFinished() {
    writeComplete = !writeComplete;
}
