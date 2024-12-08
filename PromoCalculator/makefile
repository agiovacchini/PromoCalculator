CXX		=	g++
CXXFLAGS	=	-O3 -DNDEBUG -w -std=c++17 -Wall -pthread -DBOOST_BIND_GLOBAL_PLACEHOLDERS
NLIB		=	-lboost_atomic -lboost_system -lboost_filesystem -lboost_iostreams -lboost_log -lboost_date_time -lboost_regex -lboost_log_setup -lboost_chrono -lboost_thread

BIN=promoCalculator
OBJDIR = obj

SRC=$(wildcard *.cpp)
SRC-FILES=$(filter-out stdafx.cpp, $(SRC))

OBJ = $(SRC-FILES:%.cpp=$(OBJDIR)/%.o)

all: $(OBJDIR) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BIN) $^ $(NLIB)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BIN)