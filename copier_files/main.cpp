/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
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
    /* load the file and copy it to the destination */
    return EXIT_SUCCESS;
}

bool parseCommandLine(int argc, char** argv, std::shared_ptr<reader> theReader, 
std::shared_ptr<reader> theWriter, bool* timed){

    if (argc < INPUT_MIN) {

    }
    theReader = std::make_shared<reader>(std::string(argv[1]));
    theWriter = std::make_shared<reader>(std::string(argv[2]));
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
