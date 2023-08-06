/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Reader.h"
#include "Writer.h"
#include "Timer.h"
#include <cstdlib>
#include <memory>

using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

#define STANDARD_COMMAND 3
#define CONFIG_ADDITIONAL 4
#define TIMED "-t"


int cmdError();

bool parseCommandLine(int argc, char** argv, bool* timed);

int main(int argc, char** argv) {
 
    bool* timed = new bool(false);
    bool success = parseCommandLine(argc, argv, timed);
    std::cout << "C1" << std::endl;
    if(success){
        shared_ptr<Writer> theWriter = make_shared<Writer>(string(argv[2]));
        shared_ptr<Reader> theReader = make_shared<Reader>(string(argv[1]), theWriter); 
        unique_ptr<Timer> run = make_unique<Timer>(theWriter, theReader);
        std::cout << "C1" << std::endl;
        if (*timed) { run->runTimed();
        } else run->run();
    } else {
        cmdError();
    }
    
    delete timed;
    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, bool* timed){
    bool valid = true;

    if (argc >= STANDARD_COMMAND && argc <= CONFIG_ADDITIONAL) {
            if(argc == CONFIG_ADDITIONAL){
                *timed, valid = (string(argv[3]) == TIMED);
            }       
    } else { 
        valid = false;
    }
    return valid;
}

int cmdError() {
    std::cout << 
    "Error, try following input for standard compile:\n"
    "timed: $./mtcopier infile outfile -t\n" 
    "untimed: $./mtcopier infile outfile\n" 
    << std::endl;

    return 0;
}
