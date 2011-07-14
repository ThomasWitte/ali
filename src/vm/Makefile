CC = g++
CXXFLAGS = -pipe
LDFLAGS	= 
DEBUGFLAGS = -g3 -Wall -std=c++0x
RELEASEFLAGS = -O3 -march=native -std=c++0x

OBJ =   vmtest.cpp vm.cpp assembler.cpp
BIN =   ../vmtest

release: CXXFLAGS += $(RELEASEFLAGS)
release: all

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: all

all: $(OBJ)
	$(CC) $(CXXFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $<
