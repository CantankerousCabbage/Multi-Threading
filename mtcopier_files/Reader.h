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

using std::string;
using std::shared_ptr;

class Reader {
   public:
    /* this class has a bunch of static (which means shared in a class)
     * because we need to share the state between threads. For example, we
     * are reading from the same file, should have the same locks for all
     * readers, etc.
     */

    /*
     * Constructors/Destructors
     */
    Reader();
    Reader(int ID);
    ~Reader();

    /*
     * Initialises shared data for class.
     */
    static void init(const std::string& name);

    /*
     * Implements thread. Must be static as c doesn't recognise 'this' implicit.
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
    void cleanUp();

    /**
     * Returns thread to. Used to invoke join in main.
     **/
    pthread_t* getThread();

    /**
     * Returns thread to. Used to invoke join in main.
     **/
    void setFinal();


   // private:
    static int queueCounter;
    static int readCounter;
    static bool readComplete;

    static pthread_mutex_t* appendLock;
    static pthread_mutex_t* readLock;
    static pthread_cond_t* appendCond;

    static string inFile;
    static std::ifstream in;
    

   private:
    pthread_t* readThread;
    string readLine;
    int readID;
    int threadID;
    
};
#endif
