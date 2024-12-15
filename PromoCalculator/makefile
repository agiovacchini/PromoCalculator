# Directory paths
OBJDIR = obj
INCLUDEDIR = include
SRCDIR = src
BINDIR = bin

# Source and object files
SRC = $(wildcard $(SRCDIR)/*.cpp)
SRCFILES=$(filter-out src/stdafx.cpp, $(SRC))
OBJFILES = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCFILES))

# Output binary
BIN = $(BINDIR)/promo-calculator

# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -DNDEBUG -w -std=c++17 -Wall -pthread -DBOOST_BIND_GLOBAL_PLACEHOLDERS -I$(INCLUDEDIR)
LDFLAGS = -lboost_atomic -lboost_system -lboost_filesystem -lboost_iostreams -lboost_log -lboost_date_time -lboost_regex -lboost_log_setup -lboost_chrono -lboost_thread

# Default target
all: $(BIN)

# Link the binary
$(BIN): $(OBJFILES) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BINDIR):
	@mkdir -p $(BINDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Clean up build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
