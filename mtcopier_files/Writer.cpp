/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Writer.h"
#include "Timer.h"
#include "Reader.h"

int Writer::lineCount;
int Writer::writeCount;


pthread_mutex_t* queueLock;
pthread_mutex_t* fetchLock;
pthread_mutex_t* writeLock;

pthread_cond_t* queueCond;
pthread_cond_t* writeCond;

std::ofstream Writer::out;
std::deque<std::string> Writer::queue;
std::string Writer::name;
int Writer::numThreads;
bool* Writer::timed;

void Writer::init(const std::string& name, const int& numThreads) {
    Writer::writeComplete = false;
    Writer::name = name;
    Writer::numThreads = numThreads;
    Writer::lineCount = 0;
    Writer::writeCount = 1;

    pthread_mutex_init(fetchLock, NULL);
    pthread_mutex_init(queueLock, NULL);
    pthread_mutex_init(writeLock, NULL);

    pthread_cond_init(queueCond, NULL);
    pthread_cond_init(writeCond, NULL);

    out.open(name);
}

void Writer::run() {
    write_data data[numThreads];
    pthread_t threads[numThreads];

    
    for (int i = 0; i < numThreads; ++i) {
        data[i].writeId = i;
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

void* Writer::runner(void* arg) { 
    write_data* data = (write_data*) arg;

    while (!writeComplete) {
        fetchData(data);
        writeData(data);
    }
    return nullptr; 
}

void Writer::fetchData(write_data* data){
    //If dequeue unsuccesful wait for signal
    pthread_mutex_lock(fetchLock);
    if (writeComplete) {
        pthread_mutex_unlock(queueLock);
        pthread_exit(NULL);
    } else {
        dequeue(data);
    pthread_mutex_unlock(fetchLock);

    }
}


void Writer::append(const std::string& line) {
    //Lock queue. Signal queue on puch incase of waiting threads.
    pthread_mutex_lock(queueLock);
        queue.push_back(line);
    pthread_cond_signal(queueCond);
    pthread_mutex_unlock(queueLock);
    
}

bool Writer::dequeue(write_data* arg){
    //Wait for signal if queue empty.
    pthread_mutex_lock(queueLock);
    while(!queue.size()){
        pthread_cond_wait(queueCond, queueLock);
    } 
        arg->writeLine = queue[0];
        arg->writeId = ++lineCount;
        queue.pop_front();
        if(checkFinal(arg)) setFinished();
    pthread_mutex_unlock(queueLock);
    
    return true; 
}

void Writer::writeData(write_data* data){
     pthread_mutex_lock(writeLock);

    while(data->writeId != writeCount){
        pthread_cond_wait(writeCond, writeLock);
    }
        out << data->writeLine; 
        writeCount++;    
    pthread_cond_broadcast(writeCond);
    pthread_mutex_unlock(writeLock);

}

void Writer::cleanUp() {
    pthread_mutex_destroy(queueLock);
    pthread_mutex_destroy(fetchLock);
    pthread_mutex_destroy(writeLock);
    pthread_cond_destroy(queueCond);
}

bool Writer::checkFinal(write_data* data) {
    return finalCount && *finalCount == data->writeId;
}

void Writer::setFinal(int* count)  {
    finalCount = count;
}

void Writer::setFinished() {
    
}
