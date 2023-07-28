/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#ifndef READER
#define READER

#include <pthread.h>
#include <fstream>
#include <iostream>
#include <string>
class Writer;

class Reader {
   public:
    /* this class has a bunch of static (which means shared in a class)
     * because we need to share the state between threads. For example, we
     * are reading from the same file, should have the same locks for all
     * readers, etc.
     **/

    /** initialize the shared data for the class - for example, open the
     * file. There will be other things which you will need to figure out
     * as you complete the assignment.
     **/
    static void init(const std::string& name, const int& numThreads, Writer* writer);

    /**
     * the method that implements the thread. It has to be static as the first
     * implicit argument to all instance methods is 'this' and c functions don't
     * know about classes
     **/
    static void* runner(void*);

    /**
     * does the setup for and launches the threads
     **/
    void run();
    /**
     * there may be other functions you need so declare them.
     **/

    /**
     * Clean up pointers and mutex
     **/
    void cleanUp();

   // private:
   static int queueCounter;
   static int readCounter;
   static pthread_mutex_t* appendLock;
   static pthread_mutex_t* readLock;
   static std::ifstream in;
   static std::string name;
   static int numThreads;
   static Writer* write;
   static bool* timed;
    /**
     * There may be other private instance data you need so declare those here.
     **/
};
#endif
