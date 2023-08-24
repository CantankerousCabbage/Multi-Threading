/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Reader.h"
#include "Timer.h"
#include "Writer.h"


pthread_cond_t Reader::pushCond;

shared_ptr<Timer> Reader::timer;
std::ifstream Reader::in;
string Reader::inFile;

Reader::Reader() {  
    //Holds timing data
    this->tLog =  make_shared<TimeLog>(); 
}
Reader::~Reader(){ }

bool Reader::init(const std::string& fileName, shared_ptr<Timer> timer, shared_ptr<bool> fileTest) {
    
    Reader::inFile = fileName;
    Reader::in.open(inFile);
    
    bool fileCheck = in.good();
    
    if(fileCheck) {
        Reader::timer = timer;
        pthread_cond_init(&pushCond, NULL); 
    }
    return fileCheck;
}

void Reader::run() {
    if(pthread_create(&readThread, NULL, &runner, this)){
        std::cout << "Error: unable to create thread" << std::endl;
            exit(-1);
    }
}

void* Reader::runner(void* arg) { 
    Reader* reader = (Reader*) arg;
    
    while(!Writer::finished) {  
        reader->execute();  
    }
    return NULL;
}



void Reader::execute() {
    //Blocked Push Lock timer
    if(this->timer->timed)this->tLog->startLockTimer();
    pthread_mutex_lock(&Writer::queueLock); 
    if(this->timer->timed)this->tLog->endLockTimer(this->tLog->lockOne);
    
    //Prevent errant threads exectuting post finish
    if(!Writer::finished){

        //Waiting Push timer
        if(this->timer->timed)this->tLog->startWaitTimer();
        //Conditional variable, triggered at Max buffer
        while(Writer::queue.size() == BUFFER){ 
            pthread_cond_wait(&Reader::pushCond, &Writer::queueLock);
        } 
        if(this->timer->timed)this->tLog->endWaitTimer();
        
        //Read Time timer
        if(this->timer->timed)this->tLog->startLockTimer();
        if(!std::getline(in, this->readLine)){ 
                Writer::setFinished(); 
        } else {
            Writer::append(this->readLine, this); 
        } 
        if(this->timer->timed)this->tLog->endLockTimer(this->tLog->IO);

        //If buffer full unblock read threads else continue pushing to queue
        if(Writer::queue.size() == BUFFER)
        {   
            pthread_cond_signal(&Writer::popCond);
        } else {   
            pthread_cond_signal(&Reader::pushCond);      
        }

    }
   pthread_mutex_unlock(&Writer::queueLock);         
}

void Reader::cleanUp(){ 
    pthread_cond_destroy(&pushCond);
}

bool Reader::reset(){
    Reader::in.open(inFile);
    bool fileCheck = in.good();
   
    return fileCheck;
}

void Reader::resetInstance(){
    this->readLine = "";
    this->tLog->reset();
}

pthread_t Reader::getThread(){
    return readThread;
}

void Reader::close(){
    Reader::in.close();
}

