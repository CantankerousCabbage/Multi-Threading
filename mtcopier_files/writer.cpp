/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "writer.h"

#include "reader.h"

/**
 * implement these functions requred for the writer class
 **/
void writer::init(const std::string& name, const int& numThreads) {
    writer::name = name;
    writer::numThreads = numThreads;
}

void writer::run() {}

void* writer::runner(void* arg) { return nullptr; }

void writer::append(const std::string& line) {
    queue.push_back(line);
}

void writer::setfinished() {}
