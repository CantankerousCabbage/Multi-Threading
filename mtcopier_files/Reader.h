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
using std::unique_ptr;
using std::make_shared;

class Reader {
   public:

    /*
     * Constructors/Destructors 
     */
    Reader();
    ~Reader();

    /*
     * Initialises shared data for class.
     */
    static bool init(const std::string& name, shared_ptr<Timer> timer, shared_ptr<bool> fileTest);

    /*
     * Handles thread loop, write to buffer append
     */
    static void* runner(void*);

    /*
     * Creates read thread for reader instance
     */
    void run();

    /**
     * Governs execution loop and lock and conidition for reader
     **/
    void execute();

    /**
     * Clean up pointers and pthread mutex, condition variables.
     **/
    static void cleanUp();

    /**
     * Reset for multiple runs
     **/
    static bool reset();

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
     * Resets data in reader object for another run
     **/
    void resetInstance();

    /**
     * close file
     **/
    static void close();

    friend class Writer;

    static pthread_cond_t pushCond;
    static shared_ptr<Timer> timer;
    static string inFile;
    static std::ifstream in;

        shared_ptr<TimeLog> tLog;
    protected:
        string readLine;
        
    private:
        pthread_t readThread;
    
};
#endif
