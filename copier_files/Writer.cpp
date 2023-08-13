/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "Writer.h"

/**
 * provide your implementation for the writer functions here
 **/
Writer::Writer(const std::string& name) : name{name} {
    out.open(name);
    first = true;
}

void Writer::run() {
        
        out <<  ((first) ? "" : "\n") << queue.front();
        queue.pop_front(); 
        if(first) first = !first;

}



void Writer::append(const std::string& line) {
    queue.push_back(line);
}

void Writer::close(){
    
    out.close();
    first = true;
}
