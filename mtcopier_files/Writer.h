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
#define BUFFER 50

using std::string;
using std::shared_ptr;

class Writer {
    public:

        Writer();
        Writer(int ID);
        ~Writer();
        static void init(const std::string& name, shared_ptr<Timer> timer);
        static void* runner(void*);
        static void cleanUp();
        

        /*
        * Thread safe append. Waits on conditional if dequeue holds lock.
        */
        static void append(const std::string& line, Reader* reader);

        /*
        * Handle write loop from buffer to output
        */
        void run();

        /*
        * Thread safe deque. Waits on conditional if queue empty of if append holds lock.
        */
        bool dequeue();

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
         * Returns time log
         **/
        int gettLog();


        static void setFinished();

        static int lineCount;
        static int writeCount;

        static int dequeueWait;
        static int writeWait;
        
        // static int* finalCount;
        static bool queuedComplete;
        static bool writeComplete;

        static pthread_mutex_t queueLock;
        static pthread_mutex_t writeLock;

        static pthread_cond_t queueCond;
        static pthread_cond_t writeCond;

        static shared_ptr<Timer> timer;
        static std::ofstream out;
        static std::deque<std::string> queue;

    private:
        TimeLog* tLog;
        pthread_t writeThread;
        string writeLine;
        int writeID;
        int threadID;
        
};
#endif
