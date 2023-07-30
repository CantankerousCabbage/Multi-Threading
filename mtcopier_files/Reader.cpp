/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Reader.h"
#include "Timer.h"
#include "Writer.h"


pthread_mutex_t* Reader::appendLock;
pthread_mutex_t* Reader::readLock;
pthread_cond_t* Reader::appendCondition;
std::ifstream Reader::in;
std::string Reader::name;
int Reader::numThreads;
Writer* Reader::write;
bool* Reader::timed;
bool* Reader::readComplete;

void Reader::init(const std::string& fileName, const int& numThreads, Writer* writer) {
    write = writer;
    name = fileName;
    Reader::numThreads = numThreads;
    readComplete = new bool(false);
    in.open(name);

    queueCounter = 1;
    readCounter = new int(0);

    pthread_mutex_init(readLock, NULL);
    pthread_mutex_init(appendLock, NULL);
    pthread_cond_init(appendCondition, NULL);
}

void Reader::run() {
    
    // std::cout << "threads in read: " << numThreads << std::endl;
    read_data data[numThreads];
    pthread_t threads[numThreads];

    
    for (int i = 0; i < numThreads; ++i) {
        data[i].queueId = i;
        if(pthread_create(&threads[i], NULL, &runner, &data[i])) {
            std::cout << "error: unable to create thread" << std::endl;
            exit(-1);
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        if(pthread_join(threads[i], NULL)) {
            std::cout << "error: unable to join thread" << std::endl;
            exit(-1);
        }
    }
    
    cleanUp();
    std::cout << "Reader threads joined" << std::endl;
}

void* Reader::runner(void* arg) { 
    read_data* data = (read_data*)arg;
    
    
    std::cout << "Thread ID running: " << data->queueId << "\n";
    
    //Check that eof not reached before attempting read.
    if(in.eof()) in.close();
    
    while(!(*readComplete)) {
        readLine(data);
        queueLine(data);
    }

    return NULL;
}

void Reader::readLine(read_data* data) {
    //Lock input on read
    pthread_mutex_lock(readLock);
        if(!(*readComplete) && std::getline(in, data->readLine)){ 
            data->queueId = ++(*readCounter);  
        } else {
            if(!(*readComplete)) readFinished();
            pthread_mutex_unlock(readLock);
            pthread_exit(NULL);
        }
    pthread_mutex_unlock(readLock);
}

void Reader::queueLine(read_data* data) {
    pthread_mutex_lock(appendLock);
    //Threads wait if it isn't their lines turn to append.
    while(data->queueId != queueCounter){
        pthread_cond_wait(appendCondition, appendLock);
    }
        write->append(data->readLine);
        queueCounter++;
    //On counter increment broadcast to waiting threads to recheck conition;   
    pthread_cond_broadcast(appendCondition);
    pthread_mutex_unlock(readLock);
}

void Reader::readFinished() {
    *readComplete++;
    Writer::setFinal(readCounter);
}

void Reader::cleanUp(){ 
    delete readCounter;
    delete readComplete;
    pthread_mutex_destroy(appendLock);
    pthread_mutex_destroy(readLock);
    pthread_cond_destroy(appendCondition);
}

