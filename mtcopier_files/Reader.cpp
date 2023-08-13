/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Reader.h"
#include "Timer.h"
#include "Writer.h"

int Reader::queueCounter;
int Reader::readCounter;
bool Reader::readComplete;

pthread_mutex_t Reader::appendLock;
pthread_mutex_t Reader::readLock;
pthread_cond_t Reader::appendCond;

shared_ptr<Timer> Reader::timer;
std::ifstream Reader::in;
string Reader::inFile;


Reader::Reader(){}
Reader::Reader(int ID) : threadID{ID} {
    this->tLog = (Reader::timer) ? new TimeLog() : nullptr;
}
Reader::~Reader(){
    delete tLog;
}

void Reader::init(const std::string& fileName, shared_ptr<Timer> timer) {
    
    Reader::timer = timer;
    Reader::inFile = fileName;
    Reader::in.open(inFile);

    Reader::readComplete = false;
    Reader::readCounter = 0;
    Reader::queueCounter = 1;

    Reader::appendLock;
    Reader::readLock;
    Reader::appendCond;

    pthread_mutex_init(&readLock, NULL);
    pthread_mutex_init(&appendLock, NULL);
    pthread_cond_init(&appendCond, NULL);   
}

void Reader::run() {
    pthread_create(&readThread, NULL, &runner, this);
}

void* Reader::runner(void* arg) { 
    Reader* reader = (Reader*) arg;
    
    while(!Reader::readComplete) {  
        reader->getLine();
        reader->queueLine();  
    }
   
    return NULL;
}

void Reader::getLine() {
    
    if(timer) this->tLog->startLockTimer();

    pthread_mutex_lock(&readLock); 

        
        if(!Reader::readComplete && std::getline(in, this->readLine)){  
            this->readID = ++Reader::readCounter; 
           
        } else {

            if(!Reader::readComplete) Reader::readFinished();
            pthread_mutex_unlock(&readLock);
            pthread_exit(NULL);
        }
    pthread_mutex_unlock(&readLock);  
   
   if(timer) tLog->lockOne = tLog->endLockTimer(); 
        
}

void Reader::queueLine() {
 
        Writer::append(this->readLine, this);
}

void Reader::readFinished() {
    Reader::readComplete = true;
}

void Reader::cleanUp(){ 
    
    pthread_mutex_destroy(&appendLock);
    pthread_mutex_destroy(&readLock);
    pthread_cond_destroy(&appendCond);
}

pthread_t Reader::getThread(){
    return readThread;
}

int Reader::getID(){
    return this->threadID;
}

int Reader::getReadID(){
    return this->readID;
}

