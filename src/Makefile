CXX := g++

# List of source files
SRC := $(wildcard *.cpp)

# List of obj files
OBJ := $(SRC:.cpp=.o)

# c++11 flag
CXXFLAGS := -std=c++11

# Linker files
LDLIBS := -lpthread

# Tell make that these target are not real files
.PHONY: all clean

all: $(SRC) $(OBJ)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(LDLIBS)

# Let's clean up the mess
clean:
	$(RM) $(EXE) $(OBJ) $(DEP)

# Don't forget to include the dependency files to let make know when to recompile
-include $(DEP)