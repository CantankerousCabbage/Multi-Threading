
##
 # startup code provided by Paul Miller for COSC1114 - Operating Systems
 # Principles
 ##
p1 = copier_files/
p2 = mtcopier_files/

.default: all

path1 = 

all: copier mtcopier

clean:
	rm -rf ppd *.o *.dSYM

copier: ${p1}main.o ${p1}reader.o ${p1}writer.o 
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^

mtcopier: ${p2}main.o ${p2}reader.o ${p2}writer.o
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^

%.o: %.cpp
	g++ -Wall -Werror -std=c++14 -g -O -c $^
# provide make targets here to build the two programs 
