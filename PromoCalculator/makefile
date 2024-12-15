CXX		=	g++
CXXFLAGS	=	-O3 -DNDEBUG -w -std=c++17 -Wall -pthread -DBOOST_BIND_GLOBAL_PLACEHOLDERS
NLIB		=	-lboost_atomic -lboost_system -lboost_filesystem -lboost_iostreams -lboost_log -lboost_date_time -lboost_regex -lboost_log_setup -lboost_chrono -lboost_thread

BIN=promo_calculator
OBJDIR = obj
BINDIR = bin

SRC=$(wildcard *.cpp)
SRC-FILES=$(filter-out stdafx.cpp, $(SRC))

OBJ = $(SRC-FILES:%.cpp=$(OBJDIR)/%.o)

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$(BIN) $^ $(NLIB)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -Rf $(OBJDIR)
	rm -Rf $(BINDIR)
