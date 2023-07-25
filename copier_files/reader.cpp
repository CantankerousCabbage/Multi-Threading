/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>

reader::reader(const std::string& name, writer& mywriter) : thewriter{mywriter}, name{name} {}

void reader::run() {

    std::string line;
    in.open(name);

    //TODO may need to add end check
    while (in.is_open() && std::getline(in, line)) {
        thewriter.append(line);
    }

    in.close();
}
