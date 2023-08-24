/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Reader.h"
#include "Writer.h"
#include "Timer.h"
#include <cstdlib>
#include <memory>
#include <chrono>

using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

#define STANDARD_COMMAND 3
#define CONFIG_ADDITIONAL 5
#define DEFAULT 1
#define TIMED "-t"


int cmdError();

bool parseCommandLine(int argc, char** argv, shared_ptr<bool> timed, shared_ptr<int> numRuns);

int main(int argc, char** argv) {
    
    // shared_ptr<bool> fileTest = make_shared<bool>(false);

    // success = success && Reader::init(inFile, timer, fileTest); 
    // if(success) success = success && Writer::init(outFile, timer);
    shared_ptr<bool> timed = make_shared<bool>();
    shared_ptr<int> numRuns = make_shared<int>(DEFAULT);

    bool success = parseCommandLine(argc, argv, timed, numRuns);

    std::string input = string(argv[1]);
    std::string output = string(argv[2]);

    shared_ptr<Writer> theWriter = make_shared<Writer>(output);
    shared_ptr<Reader> theReader = make_shared<Reader>(input, theWriter); 

    

    if(success){
        
        unique_ptr<Timer> run = make_unique<Timer>(theWriter, theReader);
        int runs = 0;

        while(success && runs != *numRuns) {
            success = theReader->init() && theWriter->init();
            if (success){
                if (*timed) { 
                run->runTimed();
                } else {
                    theReader->run();  
                } 
                runs++;

                if(*numRuns > DEFAULT) run->recordResults(); 
            } else {
                cmdError();
            }  
        } 
        if(success && *numRuns > DEFAULT) run->print(*numRuns); 
         
    
    } else {
        cmdError();
    }
    
    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, shared_ptr<bool> timed, shared_ptr<int> numRuns){

    auto timedCheck = [](string config){return config == TIMED;};
    bool valid = false;

    if (argc > STANDARD_COMMAND && argc <= CONFIG_ADDITIONAL) {
      
        try{   
            if(argc == CONFIG_ADDITIONAL - 1) {
                *timed = timedCheck(string(argv[3]));
                valid = *timed;
            } else if(argc == CONFIG_ADDITIONAL){
                *timed = timedCheck(string(argv[3]));
                *numRuns = atoi(argv[4]);
                valid = (*timed && *numRuns);
            }    
        }catch(std::invalid_argument){
            valid = false;
        }    
    } else {
        valid = true;
    }
    
    return valid;
}



int cmdError() {
    std::cout << 
    "Error, try the followings input for to run:\n"
    "timed: $./copier infile outfile -t\n" 
    "OR\n"
    "timed: $./copier infile outfile -t x\n" 
    "x - number of runs\n"
    "untimed: $./copier infile outfile\n\n" 
    "Please check infile exists.\n" 
    << std::endl;

    return 0;
}
