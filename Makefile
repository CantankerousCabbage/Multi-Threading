
#
#  startup code provided by Paul Miller for COSC1114 - Operating Systems
#  Principles
 #

p1 = copier_files/
p2 = mtcopier_files/

.default: all

all: copier mtcopier

clean: clean_copier clean_mtcopier

clean_copier:
	rm -rf ppd ${p1}*.o copier

clean_mtcopier:
	rm -rf ${p2}*.o mtcopier

copier: ${p1}main.o ${p1}reader.o ${p1}writer.o ${p1}timer.o
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^	-lpthread

${p1}%.o: ${p1}%.cpp
	g++ -Wall -Werror -std=c++14 -g -O -c -o $^

mtcopier: ${p2}main.o ${p2}Reader.o ${p2}Writer.o ${p2}Timer.o
	g++ -Wall -Werror -std=c++20 -g -O -o $@ $^ -lpthread

${p2}%.o: %.cpp
	g++ -Wall -Werror -std=c++20 -g -O -c -o $^ 
