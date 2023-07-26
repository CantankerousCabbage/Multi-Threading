/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
#include "timer.h"
#include <cstdlib>
#include <memory>

#define INPUT_MIN 2
#define CONFIG_ADDITIONAL 4
#define TIMED "-t"

/* global variables if needed go here */
int main(int argc, char** argv) {
    /* check command line arguments */


    std::shared_ptr<reader> theReader; 
    std::shared_ptr<writer> theWriter;
    bool* timed;

    parseCommandLine(argc, argv, theReader, theWriter, timed);
    std::unique_ptr<timer> run = std::make_unique<timer>(theWriter, theReader);

    if (*timed) { run->runTimed();
    } else run->run();

    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, std::shared_ptr<reader> theReader, 
std::shared_ptr<writer> theWriter, bool* timed){

    if (argc < INPUT_MIN) {

    }
    theReader = std::make_shared<reader>(std::string(argv[1]));
    theWriter = std::make_shared<writer>(std::string(argv[2]));
    *timed = (argc == CONFIG_ADDITIONAL && argv[3] == TIMED) ? true : cmdError();
}

int cmdError() {
    std::cout << 
    "Error, try following input for standard compile:\n"
    "timed: $./mtcopier infile outfile -t\n" 
    "untimed: $./mtcopier infile outfile -t\n" 
    << std::endl;

    return 0;
}
