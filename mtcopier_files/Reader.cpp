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
    
    // std::cout << ((Reader::timer) ? "Timer Made" : "Not MAde") << std::endl;
    this->tLog = (Reader::timer) ? new TimeLog() : nullptr;
}
Reader::~Reader(){
    delete tLog;
}

bool Reader::init(const std::string& fileName, shared_ptr<Timer> timer, shared_ptr<bool> fileTest) {
    
    Reader::inFile = fileName;
    Reader::in.open(inFile);
    
    bool fileCheck = in.good();
    
    if(fileCheck) {
        Reader::timer = timer;

        Reader::readComplete = false;
        Reader::readCounter = 0;
        Reader::queueCounter = 1;

        pthread_mutex_init(&readLock, NULL);
        pthread_mutex_init(&appendLock, NULL);
        pthread_cond_init(&appendCond, NULL); 
    }
    return fileCheck;
}

void Reader::run() {
    //Add error handling
    if(pthread_create(&readThread, NULL, &runner, this)){
        std::cout << "Error: unable to create thread" << std::endl;
            exit(-1);
    }
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
    // std::cout << "Timer: 1 Exit" << (this->tLog->lockOne) << std::endl;
    pthread_mutex_lock(&readLock); 
    // std::cout << "Timer: 2" << std::endl;
    if(timer) this->tLog->endLockTimer(this->tLog->lockOne); 
        
        if(!Reader::readComplete && std::getline(in, this->readLine)){  
            this->readID = ++Reader::readCounter; 
           
        } else {

            if(!Reader::readComplete) Reader::readFinished();
            pthread_mutex_unlock(&readLock);
            pthread_exit(NULL);
        }
    pthread_mutex_unlock(&readLock);  
   
   
        
}

void Reader::queueLine() {
 
        Writer::append(this->readLine, this);
}

void Reader::readFinished() {
    in.close();
    Reader::readComplete = true;
}

void Reader::cleanUp(){ 
    
    pthread_mutex_destroy(&appendLock);
    pthread_mutex_destroy(&readLock);
    pthread_cond_destroy(&appendCond);
}

bool Reader::reset(){
    Reader::in.open(inFile);
    bool fileCheck = in.good();
    if (fileCheck) {
        Reader::readComplete = false;
        Reader::readCounter = 0;
        Reader::queueCounter = 1;    
    }
    return fileCheck;
}

void Reader::resetInstance(){
    this->readLine = "";
    this->readID = 0;
    delete this->tLog;
    this->tLog = new TimeLog();
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

