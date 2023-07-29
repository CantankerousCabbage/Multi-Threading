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

std::ofstream Writer::out;
std::deque<std::string> Writer::queue;
std::string Writer::name;
int Writer::numThreads;
bool* Writer::timed;

void Writer::init(const std::string& name, const int& numThreads) {
    Writer::name = name;
    Writer::numThreads = numThreads;
    Writer::lineCount = 0;
    Writer::writeCount = 0;

    pthread_mutex_init(fetchLock, NULL);
    pthread_mutex_init(queueLock, NULL);
    pthread_mutex_init(writeLock, NULL);

    pthread_cond_init(fetchCond, NULL);
    pthread_cond_init(queueCond, NULL);

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
    while(pthread_mutex_trylock(fetchLock) != UNLOCKED){
        pthread_cond_wait(fetchCond, fetchLock);
    } 
        while(!dequeue(data)) {
            pthread_cond_wait(queueCond, queueLock);
        }
    pthread_mutex_unlock(fetchLock);
    pthread_cond_signal(fetchCond);
    
    return nullptr; 
}

void Writer::append(const std::string& line) {
    while(pthread_mutex_trylock(queueLock)){
        pthread_cond_wait(queueCond, queueLock);
    }
        queue.push_back(line);
    pthread_mutex_unlock(queueLock);
    pthread_cond_signal(queueCond);
}

bool Writer::dequeue(write_data* arg){
    if (queue.size() && !pthread_mutex_trylock(queueLock)) {
            arg->writeLine = queue[0];
            arg->writeId = lineCount++;
            queue.pop_front();
        pthread_mutex_unlock(queueLock);
        pthread_cond_signal(queueCond);
    } else return false;
    return true; 
}

void Writer::cleanUp() {
    pthread_mutex_destroy(queueLock);
    pthread_mutex_destroy(fetchLock);
    pthread_mutex_destroy(writeLock);
    pthread_cond_destroy(fetchCond);
    pthread_cond_destroy(queueCond);
}

void Writer::setfinished() {}
