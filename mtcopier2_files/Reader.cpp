/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Reader.h"
#include "Timer.h"
#include "Writer.h"

pthread_mutex_t Reader::appendLock;
pthread_mutex_t Reader::readLock;
pthread_cond_t Reader::pushCond;

shared_ptr<Timer> Reader::timer;
std::ifstream Reader::in;
string Reader::inFile;

Reader::Reader() : valid{true} {
    // std::cout << ((Reader::timer) ? "Timer Made" : "Not MAde") << std::endl;
    this->tLog =  make_shared<TimeLog>();
    // this->valid = true;
}
Reader::~Reader(){ }

bool Reader::init(const std::string& fileName, shared_ptr<Timer> timer, shared_ptr<bool> fileTest) {
    
    Reader::inFile = fileName;
    Reader::in.open(inFile);
    
    bool fileCheck = in.good();
    
    if(fileCheck) {
        Reader::timer = timer;
        pthread_mutex_init(&readLock, NULL);
        pthread_mutex_init(&appendLock, NULL);
        pthread_cond_init(&pushCond, NULL); 
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
    
    while(!Writer::finished) {  
        reader->getLine();  
    }
//    delete reader;
    return NULL;
}



void Reader::getLine() {
    
    this->tLog->startLockTimer();
    pthread_mutex_lock(&readLock); 
    this->tLog->endLockTimer(this->tLog->lockOne); 
        
        // if(!Writer::finished){
        //     if(!std::getline(in, this->readLine)){  
        //         std::cout << "Couldn't getLine" << std::endl;
        //         if(!Writer::finished){
        //             Writer::setFinished(); 
        //             std::cout << "FINISHED" << std::endl;  
        //         } 
        //         pthread_mutex_unlock(&Reader::readLock);
        //         pthread_exit(NULL);   
        //     } 
        //     std::cout << "Appending" << std::endl;
        //     Writer::append(this->readLine, this); 
        // } else {
        //     std::cout << "Outer exit" << std::endl;
        //     pthread_mutex_unlock(&Reader::readLock);
        //     pthread_exit(NULL); 
        // } 

        // if(Writer::finished || !std::getline(in, this->readLine)){
        //     if(!Writer::finished) Writer::setFinished();
        //     this->valid = false;
        // }

        // Writer::append(this->readLine, this);
        // if(!Writer::finished)Writer::append(this);   

        if(!std::getline(in, this->readLine)){
            if(!Writer::finished){
                Writer::setFinished();
            } 
            pthread_cond_broadcast(&Reader::pushCond);
            pthread_cond_broadcast(&Writer::popCond);
            pthread_mutex_unlock(&Reader::readLock);
            pthread_exit(NULL);
            //  pthread_broadcast_signal(&Writer::popCond);
            // pthread_cond_signal(&Reader::pushCond); 

        }

        Writer::append(this->readLine, this);    
}

void Reader::cleanUp(){ 
    pthread_mutex_destroy(&appendLock);
    pthread_mutex_destroy(&readLock);
    pthread_cond_destroy(&pushCond);
}

bool Reader::reset(){
    Reader::in.open(inFile);
    bool fileCheck = in.good();
   
    return fileCheck;
}

void Reader::resetInstance(){
    this->readLine = "";
    this->valid = true;
    this->tLog->reset();
}

pthread_t Reader::getThread(){
    return readThread;
}

void Reader::close(){
    Reader::in.close();
}

