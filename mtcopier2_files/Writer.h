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
        static void* runner(void*);
        static void cleanUp();

        /**
         * Resets writer for multiple runs
         **/
        static bool reset();
        
        /*
        * Thread safe append. Waits on conditional if dequeue holds lock.
        */
        static void append(const std::string& line, Reader* reader);

        /*
        * Handle write loop from buffer to output
        */
        void run();

        void execute();

        /*
        * Thread safe deque. Waits on conditional if queue empty of if append holds lock.
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
         * Returns thread to. Used to invoke join in main.
         **/
        int getID();

        /**
         * Resets writer for multiple runs
         **/
        void resetInstance();

        /**
         * Upates writer that all lines have transitioned from input to buffer.
         **/
        static void setFinished();

        /**
         * close output file
         **/
        static void close();

        static int dequeueWait;
        static int pushWait;
        
        // static int* finalCount;
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
