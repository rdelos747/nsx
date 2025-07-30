CXX = g++
CXX_FLAGS = -Wall

TARG = ./bin/nsx
SRCS = $(shell find $(SRC) -name *.cpp)
LIBS = -lncurses -lpanel

all:
	$(CXX) $(CXX_FLAGS) $(SRCS) $(LIBS) -o $(TARG)

