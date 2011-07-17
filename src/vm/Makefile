CC = g++
CXXFLAGS = -pipe
LDFLAGS	= 
DEBUGFLAGS = -g3 -Wall
RELEASEFLAGS = -O3 -march=native

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
