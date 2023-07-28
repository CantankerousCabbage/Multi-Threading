/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Reader.h"
#include "Timer.h"
#include "Writer.h"

pthread_mutex_t* Reader::appendLock;
pthread_mutex_t* Reader::readLock;
std::ifstream Reader::in;
std::string Reader::name;
int Reader::numThreads;
Writer* Reader::write;
bool* Reader::timed;

void Reader::init(const std::string& fileName, const int& numThreads, Writer* writer) {
    write = writer;
    name = fileName;
    Reader::numThreads = numThreads;
    in.open(name);
    queueCounter = 0;
    readCounter = 0;
    pthread_mutex_init(readLock, NULL);
    pthread_mutex_init(appendLock, NULL);
}

void Reader::run() {
    int check;
    std::cout << "threads in read: " << numThreads << std::endl;
   read_data data[numThreads];
    pthread_t threads[numThreads];

     std::cout << numThreads << std::endl;
    for (int i = 0; i < numThreads; ++i) {
        data[i].queueId = i;
        check = pthread_create(&threads[i], NULL, &runner, &data[i]);
        if(check) {
            std::cout << "error: unable to create thread" << std::endl;
            exit(-1);
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        check = pthread_join(threads[i], NULL);
        if(check) {
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
    pthread_mutex_lock(readLock);
        if(in.is_open()){
            std::getline(in, data->readLine); 
            data->queueId = readCounter++;  
        } else {
            pthread_exit(NULL);
        }
    pthread_mutex_unlock(readLock);

    pthread_mutex_lock(appendLock);
    //Check queueCounter
        write->append(data->readLine);

    pthread_mutex_unlock(readLock);

    return (void*) data;
}

void Reader::cleanUp(){

    pthread_mutex_destroy(appendLock);
    pthread_mutex_destroy(readLock);

}

