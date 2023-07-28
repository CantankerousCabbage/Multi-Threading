/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Writer.h"
#include "Reader.h"

// class Reader;

std::ofstream Writer::out;
std::deque<std::string> Writer::queue;
std::string Writer::name;
int Writer::numThreads;
bool* Writer::timed;

/**
 * implement these functions requred for the writer class
 **/
void Writer::init(const std::string& name, const int& numThreads) {
    Writer::name = name;
    Writer::numThreads = numThreads;
}

void Writer::run() {}

void* Writer::runner(void* arg) { return nullptr; }

void Writer::append(const std::string& line) {
    queue.push_back(line);
}

void Writer::setfinished() {}
