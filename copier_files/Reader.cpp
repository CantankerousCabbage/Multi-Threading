/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Reader.h"
#include <functional>

Reader::Reader(const std::string& name, shared_ptr<Writer> mywriter) : theWriter{mywriter}, name{name} {}

void Reader::run() {

    std::string line;
    in.open(name);

    //TODO may need to add end check
    while (std::getline(in, line)) {
        theWriter->append(line);
        theWriter->run();
    }

    in.close();
    theWriter->close();
}
