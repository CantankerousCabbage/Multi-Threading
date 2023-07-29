/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Reader.h"
#include "Timer.h"
#include "Writer.h"


pthread_mutex_t* Reader::appendLock;
pthread_mutex_t* Reader::readLock;
pthread_cond_t* Reader::readCondition;
pthread_cond_t* Reader::appendCondition;
std::ifstream Reader::in;
std::string Reader::name;
int Reader::numThreads;
Writer* Reader::write;
bool* Reader::timed;
bool Reader::pass;

void Reader::init(const std::string& fileName, const int& numThreads, Writer* writer) {
    write = writer;
    name = fileName;
    Reader::numThreads = numThreads;
    pass = false;
    in.open(name);

    queueCounter = 0;
    readCounter = 0;

    pthread_mutex_init(readLock, NULL);
    pthread_mutex_init(appendLock, NULL);
    pthread_cond_init(readCondition, NULL);
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
    
    if(in.eof()) in.close();
    
    while(pthread_mutex_trylock(readLock) != UNLOCKED){
        pthread_cond_wait(readCondition, readLock);
    } 
        if(in.is_open()){
            std::getline(in, data->readLine); 
            data->queueId = readCounter++;  
        } else {
            pthread_exit(NULL);
            pthread_cond_broadcast(readCondition);
        }
    pthread_mutex_unlock(readLock);
    pthread_cond_signal(readCondition);


    pthread_mutex_lock(appendLock);
    //Threads wait if it isn't their lines turn to append.
    while(data->queueId != queueCounter){
        pthread_cond_wait(appendCondition, appendLock);
    }
        write->append(data->readLine);
        queueCounter++;
    pthread_mutex_unlock(readLock);
    //On counter increment broadcast to waiting threads to recheck conition;
    pthread_cond_broadcast(appendCondition);

    return (void*) data;
}

void Reader::cleanUp(){
    
    pthread_mutex_destroy(appendLock);
    pthread_mutex_destroy(readLock);
    pthread_cond_destroy(readCondition);
    pthread_cond_destroy(appendCondition);
}

