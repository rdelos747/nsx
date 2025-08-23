CXX = g++
CXX_FLAGS = -Wall -w

TARG = ./bin/nsx
SRC = ./src
SRCS = $(shell find $(SRC) -name *.cpp)
LIBS = -lncurses -lpanel

all:
	$(CXX) $(CXX_FLAGS) $(SRCS) $(LIBS) -o $(TARG)

