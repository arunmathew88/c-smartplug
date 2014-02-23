# makefile (use -pg for profiling)

IDIR = include
LDIR = deps
SDIR = src
ODIR = bin
TDIR = test
ZIDIR = $(LDIR)/zeromq-4.0.3/include
CC = g++
DEBUG = -g
PROFILE =
CFLAGS = -Wall -c $(DEBUG) -I$(IDIR) -I$(ZIDIR) -L$(LPATH) -Wno-unused-function
LPATH = $(LDIR)/zeromq-4.0.3/src
LIBS = -lm -lpthread -std=c++0x -lzmq

# header files => .cpp files
_DEPS = mc.h common.h slidingmc.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
_OBJ1 = mc.o house.o slidingmc.o
OBJ1 = $(patsubst %,$(ODIR)/%,$(_OBJ1))

_OBJ2 = broker.o
OBJ2 = $(patsubst %,$(ODIR)/%,$(_OBJ2))

_OBJ3 = main_test.o slidingmc.o
OBJ3 = $(patsubst %,$(ODIR)/%,$(_OBJ3))

_TOBJ = slidingmc_test.o
TOBJ = $(patsubst %,$(ODIR)/%,$(_TOBJ))

$(ODIR)/%_test.o: $(TDIR)/%_test.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

all: setup $(ODIR)/broker $(ODIR)/house $(ODIR)/main

setup:
	mkdir -p $(ODIR)
ifeq "$(wildcard $(LDIR) )" ""
	mkdir -p $(LDIR)
	cd $(LDIR)/ && wget --tries=3 http://download.zeromq.org/zeromq-4.0.3.tar.gz
	cd $(LDIR)/ && tar -zxvf zeromq-4.0.3.tar.gz
	cd $(LDIR)/zeromq-4.0.3 && ./configure && make && sudo make install && sudo ldconfig
	cd $(IDIR)/ && rm -f zmq.hpp && wget https://raw2.github.com/zeromq/cppzmq/master/zmq.hpp
	cd $(IDIR)/ && rm -f zhelpers.hpp && wget https://raw2.github.com/imatix/zguide/master/examples/C++/zhelpers.hpp
	sudo apt-get install libcppunit-dev
endif

$(ODIR)/house: $(OBJ1)
	$(CC) -I$(IDIR) -I$(ZIDIR) -o $@ $^ $(PROFILE) $(LIBS)

$(ODIR)/broker: $(OBJ2)
	$(CC) -I$(IDIR) -I$(ZIDIR) -o $@ $^ $(PROFILE) $(LIBS)

$(ODIR)/main: $(OBJ3)
	$(CC) -I$(IDIR) -I$(ZIDIR) -o $@ $^ $(PROFILE) $(LIBS)

test: setup $(TOBJ)
	@$(foreach test,$(TOBJ), $(CC) -o $(patsubst %_test.o,%,$(test)) $(test) -lcppunit && ./$(patsubst %_test.o,%,$(test));)

clean:
	rm -rf $(ODIR) *~

distclean: clean
	rm -rf $(LDIR)
	rm -f include/zmq.hpp
	rm -f include/zhelpers.hpp

rebuild: distclean all

.PHONY: clean test
