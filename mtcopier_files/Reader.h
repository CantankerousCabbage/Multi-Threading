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
#include <memory>
#include "Timer.h"

using std::string;
using std::shared_ptr;

class Reader {
   public:

    /*
     * Constructors/Destructors 
     */
    Reader();
    Reader(int ID);
    ~Reader();

    /*
     * Initialises shared data for class.
     */
    static void init(const std::string& name, shared_ptr<Timer> timer);

    /*
     * Handles thread loop, write to buffer append
     */
    static void* runner(void*);

    /*
     * Sets up and launches threads.
     */
    void run();

    /**
     * Clean up pointers and pthread mutex, condition variables.
     **/
    void getLine();

    /**
     * Clean up pointers and pthread mutex, condition variables.
     **/
    void queueLine();

    /**
     * Sets read as finished, notifies write of final line counter;
     **/
    static void readFinished();

    /**
     * Clean up pointers and pthread mutex, condition variables.
     **/
    static void cleanUp();

    /**
     * Returns thread to. Used to invoke join in main.
     **/
    pthread_t getThread();

    /**
     * Returns thread to. Used to invoke join in main.
     **/
    int getID();

    /**
     * Returns thread to. Used to invoke join in main.
     **/
    int getReadID();

    /**
     * Returns time log
     **/
    int gettLog();

    friend class Writer;

    static int queueCounter;
    static int readCounter;
    static bool readComplete;
    static pthread_mutex_t appendLock;
    static pthread_mutex_t readLock;
    static pthread_cond_t appendCond;
    static shared_ptr<Timer> timer;
    static string inFile;
    static std::ifstream in;

    protected:
        TimeLog* tLog;

    private:
        pthread_t readThread;
        string readLine;
        int readID;
        int threadID;
    
};
#endif
