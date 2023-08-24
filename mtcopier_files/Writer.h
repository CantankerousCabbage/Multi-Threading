/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#ifndef WRITER
#define WRITER

#include <pthread.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include "Timer.h"

#define FIRST 1
#define INITIAL 0
#define BUFFER 800

using std::string;
using std::shared_ptr;
using std::make_shared;

class Writer {
    public:

        Writer();
        ~Writer();
        static bool init(const std::string& name, shared_ptr<Timer> timer);

        /**
         * Writer thread function
         **/
        static void* runner(void*);

        /**
         * Destroy mutex and conditional variables
         **/
        static void cleanUp();

        /**
         * Resets for multiple runs
         **/
        static bool reset();
        
        /*
        * Adds line to buffer. Invoked from reader.
        */
        static void append(const std::string& line, Reader* reader);

        /*
        * Create writer thread
        */
        void run();

        /*
        * Handle write loop, mutex and conditionals
        */
        void execute();

        /*
        * Manages dequeue and signalling of pop condition based on buffer state
        */
        void dequeue();

        /*
        * Write data to output file
        */
        void writeData();

        /**
         * Returns thread for join.
         **/
        pthread_t getThread();

        /**
         * Resets writer instance for multiple runs
         **/
        void resetInstance();

        /**
         * Updates bool that all reads have completed.
         **/
        static void setFinished();

        /**
         * close output file
         **/
        static void close();

        static bool empty;
        static bool finished;
        static bool first;

        static pthread_mutex_t queueLock;
        static pthread_mutex_t writeLock;

        static pthread_cond_t popCond;
        static pthread_cond_t writeCond;

        static shared_ptr<Timer> timer;
        static string outName;
        static std::ofstream out;
        static std::deque<std::string> queue;
        shared_ptr<TimeLog> tLog;

    private:
        pthread_t writeThread;
        string writeLine;
        
};
#endif
