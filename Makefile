
#
#  startup code provided by Paul Miller for COSC1114 - Operating Systems
#  Principles
 #

p1 = copier_files/
p2 = mtcopier_files/
p3 = mtcopier2_files/

.default: all

all: copier mtcopier mtcopier2

clean: clean_copier clean_mtcopier clean_mtcopier2

clean_copier:
	rm -rf ppd ${p1}*.o copier

clean_mtcopier:
	rm -rf ${p2}*.o mtcopier

clean_mtcopier2:
	rm -rf ${p3}*.o mtcopier

copier: ${p1}main.o ${p1}Reader.o ${p1}Writer.o ${p1}Timer.o
	g++ -Wall -Werror -std=c++20 -g -O -o $@ $^	-lpthread

${p1}%.o: %.cpp
	g++ -Wall -Werror -std=c++20 -g -O -c -o $^

mtcopier: ${p2}main.o ${p2}Reader.o ${p2}Writer.o ${p2}Timer.o
	g++ -Wall -Werror -std=c++20 -g -O -o $@ $^ -lpthread

${p2}%.o: %.cpp
	g++ -Wall -Werror -std=c++20 -g -O -c -o $^ 

mtcopier2: ${p3}main.o ${p3}Reader.o ${p3}Writer.o ${p3}Timer.o
	g++ -Wall -Werror -std=c++20 -g -O -o $@ $^ -lpthread

${p3}%.o: %.cpp
	g++ -Wall -Werror -std=c++20 -g -O -c -o $^ 


