/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "writer.h"

/**
 * provide your implementation for the writer functions here
 **/
writer::writer(const std::string& name) : name{name} {}

void writer::run() {
    out.open(name);

    while (queue.size()) {
       out << queue.front();
       queue.pop_front(); 
    }

    out.close();
}

void writer::append(const std::string& line) {
    queue.push_back(line);
}
