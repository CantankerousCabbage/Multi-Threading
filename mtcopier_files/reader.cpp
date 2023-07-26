/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"

/**
 * implement the functions needed for this class
 **/
// std::ifstream reader::in;

void reader::init(const std::string& name, const int& numThreads) {
    reader::name = name;
    reader::numThreads = numThreads;
}

void reader::run() {

    in.open(name);
    int arg;
    pthread_t threads[reader::numThreads];
    for (unsigned i = 0; i < reader::numThreads; ++i) {
        pthread_create(&threads[i], NULL, runner, &arg);
    }
}

void* reader::runner(void* arg) { return nullptr; }
