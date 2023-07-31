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

pthread_mutex_t* Reader::appendLock;
pthread_mutex_t* Reader::readLock;
pthread_cond_t* Reader::appendCond;

std::ifstream Reader::in;
string Reader::inFile;


Reader::Reader(){}
Reader::Reader(int ID) : threadID{ID} {}
Reader::~Reader(){}

void Reader::init(const std::string& fileName) {
     
    Reader::inFile = fileName;
    Reader::in.open(inFile);

    // shared_ptr<bool> readComplete = std::make_shared<bool>(false);
    // shared_ptr<int> readCounter = std::make_shared<int>(0);
    // shared_ptr<int> queueCounter = std::make_shared<int>(0);  

    Reader::readComplete = false;
    Reader::readCounter = 0;
    Reader::queueCounter = 1;

    Reader::appendLock;
    Reader::readLock;
    Reader::appendCond;

    pthread_mutex_init(readLock, NULL);
    pthread_mutex_init(appendLock, NULL);
    pthread_cond_init(appendCond, NULL);
      
}

void Reader::run() {
 
    pthread_create(readThread, NULL, &runner, this);
    cleanUp();
}

void* Reader::runner(void* arg) { 
    Reader* reader = (Reader*) arg;
    
    //Check that eof not reached before attempting read.
    // if(in.eof()) {
    //   in.close();  
    // } 
    
    while(!Reader::readComplete) {  
        reader->getLine();
        reader->queueLine();  
    }

    return NULL;
}

void Reader::getLine() {
    //Lock input on read
    pthread_mutex_lock(readLock);   
        if(!Reader::readComplete && std::getline(in, this->readLine)){    
            this->readID = ++Reader::readCounter;  
        } else {
            if(!Reader::readComplete) Reader::readFinished();
            pthread_mutex_unlock(readLock);
            pthread_exit(NULL);
        }
    pthread_mutex_unlock(readLock);
}

void Reader::queueLine() {
    pthread_mutex_lock(appendLock);
    //Threads wait if it isn't their lines turn to append.
    while(this->readID != Reader::queueCounter){
        pthread_cond_wait(appendCond, appendLock);
    }   
        Writer::append(this->readLine);
        Reader::queueCounter++;
    //On counter increment broadcast to waiting threads to recheck conition;   
    pthread_cond_broadcast(appendCond);
    pthread_mutex_unlock(appendLock);
}

//TODO FIX
void Reader::readFinished() {
    Reader::readComplete = !Reader::readComplete;
    Writer::setFinished();
}



void Reader::cleanUp(){ 
    
    pthread_mutex_destroy(appendLock);
    pthread_mutex_destroy(readLock);
    pthread_cond_destroy(appendCond);

    // delete appendLock;
    // delete readLock;
    // delete appendCond;

    // delete readCounter;
    // delete readComplete;
}

pthread_t* Reader::getThread(){
    return readThread;
}

