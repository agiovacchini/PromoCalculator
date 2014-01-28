CXX		=	g++.exe
CXXFLAGS	=	-g -w -std=c++11 -Wall -L/lib -I/usr/local/include 
NLIB		=	-lboost_system -lboost_filesystem -lboost_iostreams -lboost_thread
#NLIBW		=	/lib/libboost_thread-mt.dll.a /lib/libboost_iostreams-mt.dll.a /lib/libboost_filesystem-mt.dll.a /lib/libboost_system-mt.dll.a

BIN=promoCalculator

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(NLIBW) $^

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)

