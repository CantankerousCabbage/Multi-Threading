/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Writer.h"

/**
 * provide your implementation for the writer functions here
 **/
Writer::Writer(const std::string& name) : name{name} {}

void Writer::run() {
    out.open(name);

    while (queue.size()) {
       out << queue.front();
       queue.pop_front(); 
    }

    out.close();
}

void Writer::append(const std::string& line) {
    queue.push_back(line);
}
