# Name of executable
EXE := leader

# Source files
SRC := $(wildcard *.cpp)

# Object files
OBJ := $(SRC:.cpp=.o)

# Dependencies
DEP := $(wildcard ../../src/*.o)

# Specify preprocessor flags (this is a built-in variable)
CPPFLAGS := -I../include
# Required flags to enable the automatic dependency generation by the compiler
CPPFLAGS += -MMD -MP

# c++11 flags
CXXFLAGS := -std=c++11

# pthread library
LDLIBS := -lpthread

# Tell make that these target are not real files
.PHONY: all clean

# Now the standard primary rule
all: $(EXE)

# Create binary
$(EXE): $(SRC) $(DEP)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)