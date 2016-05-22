LibScanner: LibScanner.cpp
	g++ -Wall -std=c++11 LibScanner.cpp -o scanner -lboost_system -lboost_filesystem

default: LibScanner

clean:
	rm scanner