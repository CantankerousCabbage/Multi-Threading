/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Writer.h"
#include <fstream>
#include <iostream>
#include <string>
#include <memory>

using std::shared_ptr;

#ifndef READER
#define READER

class Reader {
   public:
    /* create a reader that reads each line of the file and appends it to the
     * writer's queue
     */
    Reader(const std::string& name, shared_ptr<Writer> mywriter);
    /* perform the reading from the file */
    void run();
     /* Validate read file */
    bool init();
    
   private:
    std::ifstream in;
    const std::string& name;
    shared_ptr<Writer> theWriter;
};
#endif
