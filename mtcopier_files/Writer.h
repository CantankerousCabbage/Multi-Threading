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

#define FIRST 1

using std::string;

//Thread data structure to hold string as well as queue counter
typedef struct {
    int writeId;
    std::string writeLine;
} write_data;

class Writer {
    public:
        /**
         * Please note that methods and data need to be static as there's a
         * variety of information that needs to be coordinated between writers
         * such as locks and other shared data. Also, as the pthread library
         * is a c library, it does not know about classes so ruuner in particular
         * needs to be static. You can pass in instances into the function as
         * pointers though.
         **/

        Writer();
        Writer(int ID);
        ~Writer();
        static void init(const std::string& name);
        static void* runner(void*);
        void cleanUp();
        void run();

        /*
        * Thread safe append. Waits on conditional if dequeue holds lock.
        */
        static void append(const std::string& line);

        /*
        * Thread safe deque. Waits on conditional if queue empty of if append holds lock.
        */
        bool dequeue();

        /*
        * Fetch data from queue for writing
        */
        void fetchData();

        /*
        * Write data to output file
        */
        void writeData();

        // /*
        // * Sets the final line number
        // */
        // static void setFinal(int* finalCounter);

        /*
        * Check if final line received
        */
        bool checkFinal(write_data* data);

        /**
         * Returns thread to. Used to invoke join in main.
         **/
        pthread_t getThread();

        /**
         * Returns thread to. Used to invoke join in main.
         **/
        int getID();


        static void setFinished();

        static int lineCount;
        static int writeCount;
        // static int* finalCount;
        static bool finished;
        static bool noData;

        static pthread_mutex_t queueLock;
        static pthread_mutex_t fetchLock;
        static pthread_mutex_t writeLock;

        static pthread_cond_t queueCond;
        static pthread_cond_t writeCond;

        static std::ofstream out;
        static std::deque<std::string> queue;
        static std::string outFile;

    private:
        pthread_t writeThread;
        string writeLine;
        int writeID;
        int threadID;
        
};
#endif
