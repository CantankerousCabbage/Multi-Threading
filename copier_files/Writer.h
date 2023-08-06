/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include <fstream>
#include <iostream>
#include <string>
#include <deque>
#include <memory>

using std::shared_ptr;

#ifndef WRITER
#define WRITER

class Writer {
   public:
    /**
     * creates the writer instance that writes out to the file
     **/
    Writer(const std::string& name);
    /**
     * does the actual writing
     **/
    void run();
    /**
     * appends a line from the file read in to be output ... needed by the
     * reader class
     **/
    void append(const std::string& line);


   private:
    std::ofstream out;
    const std::string name;
    std::deque<std::string> queue;
};
#endif
