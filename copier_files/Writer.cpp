/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Writer.h"

/**
 * provide your implementation for the writer functions here
 **/
Writer::Writer(const std::string& name) : name{name} {
    // out.open(name);
    first = true;
}

void Writer::run() {
        //After first write prepends newline. Prevents trailing "\n" on final write
        out <<  ((first) ? "" : "\n") << queue.front();
        queue.pop_front(); 
        if(first) first = !first;
}

bool Writer::init() {
    out.open(name);
    return out.good();
}

void Writer::append(const std::string& line) {
    queue.push_back(line);
}

void Writer::close(){
    out.close();
    first = true;
}
