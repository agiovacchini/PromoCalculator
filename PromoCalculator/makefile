CXX		=	g++
CXXFLAGS	=	-O3 -DNDEBUG -w -std=c++11 -Wall -L/home/utente/ws-cpp/libs/boost_1_60_0/stage/lib -I/home/utente/ws-cpp/libs/boost_1_60_0.build/include -pthread 
NLIB		=	-lboost_atomic-gcc49-mt-1_60 -lboost_system-gcc49-mt-1_60 -lboost_filesystem-gcc49-mt-1_60 -lboost_iostreams-gcc49-mt-1_60 -lboost_log-gcc49-mt-1_60 -lboost_date_time-gcc49-mt-1_60 -lboost_regex-gcc49-mt-1_60 -lboost_log_setup-gcc49-mt-1_60 -lboost_chrono-gcc49-mt-1_60 -lboost_thread-gcc49-mt-1_60

BIN=promoCalculator

SRC=$(wildcard *.cpp)
SRC-FILES=$(filter-out stdafx.cpp, $(SRC))

OBJ=$(SRC-FILES:%.cpp=%.o)

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BIN) $^  $(NLIB)

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)

