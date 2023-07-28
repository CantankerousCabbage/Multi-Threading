
##
 # startup code provided by Paul Miller for COSC1114 - Operating Systems
 # Principles
 ##
p1 = copier_files/
p2 = mtcopier_files/

.default: all

all: mtcopier

# mtcopier: mtcopier

# copier: copier

clean:
	rm -rf ppd ${p2}*.o *.dSYM

# copier: ${p1}main.o ${p1}reader.o ${p1}writer.o ${p1}timer.o
# 	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^ 

# copier: ${p1}%.o: %.cpp
# 	g++ -Wall -Werror -std=c++14 -g -O -c $^ -lpthread

mtcopier: ${p2}main.o ${p2}Reader.o ${p2}Writer.o ${p2}Timer.o
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^ -lpthread


# mtcopier: main.o Reader.o Writer.o Timer.o
# 	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^ -lpthread

${p2}%.o: %.cpp
	g++ -Wall -Werror -std=c++14 -g -O -c $^ 
# provide make targets here to build the two programs 
