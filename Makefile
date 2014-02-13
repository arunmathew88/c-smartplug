# makefile (use -pg for profiling)

IDIR = include
CC = g++
DEBUG = -g
PROFILE =
CFLAGS = -Wall -c $(DEBUG) -I$(IDIR) -L$(LPATH) -Wno-unused-function

SDIR = src
ODIR = bin
LDIR = deps
LPATH = $(LDIR)/zeromq-4.0.3/src
LIBS = -lm -lpthread -std=c++0x -lzmq

# header files => .cpp files
_DEPS = mc.h common.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
_OBJ1 = mc.o house.o
OBJ1 = $(patsubst %,$(ODIR)/%,$(_OBJ1))

_OBJ2 = broker.o
OBJ2 = $(patsubst %,$(ODIR)/%,$(_OBJ2))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

all: setup $(ODIR)/broker $(ODIR)/house

setup:
	mkdir -p $(ODIR)
ifeq "$(wildcard $(LDIR) )" ""
	mkdir -p $(LDIR)
	cd $(LDIR)/ && wget --tries=3 http://download.zeromq.org/zeromq-4.0.3.tar.gz
	cd $(LDIR)/ && tar -zxvf zeromq-4.0.3.tar.gz
	cd $(LDIR)/zeromq-4.0.3 && ./configure && make
	cd &(IDIR)/ && wget https://raw2.github.com/zeromq/cppzmq/master/zmq.hpp
	cd &(IDIR)/ && wget https://raw2.github.com/imatix/zguide/master/examples/C++/zhelpers.hpp
endif

$(ODIR)/house: $(OBJ1)
	$(CC) -I$(IDIR) -o $@ $^ $(PROFILE) $(LIBS)

$(ODIR)/broker: $(OBJ2)
	$(CC) -I$(IDIR) -o $@ $^ $(PROFILE) $(LIBS)

clean:
	rm -rf $(ODIR) *~

distclean: clean
	rm -rf $(LDIR)
	rm -f include/zmq.hpp
	rm -f include/zhelpers.hpp

rebuild: distclean all

.PHONY: clean
