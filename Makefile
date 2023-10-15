# Variables
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Executable name
EXEC = dna_codec

# Source and object files
SRC = dna_codec.cpp
OBJ = $(SRC:.cpp=.o)

# Build rules
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rules
clean:
	rm -f $(OBJ) $(EXEC)

