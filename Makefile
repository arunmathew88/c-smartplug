# makefile (use -pg for profiling)

IDIR = include
CC = g++
DEBUG = -O3
PROFILE =
CFLAGS = -Wall -c -std=c++0x $(DEBUG) -I$(IDIR) $(PROFILE)

SDIR = src
ODIR = bin
TDIR = test
LIBS = -lm -lpthread -std=c++0x

# header files => .cpp files
_DEPS = mc.h common.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
_OBJ1 = mc.o house.o
OBJ1 = $(patsubst %,$(ODIR)/%,$(_OBJ1))

_OBJ2 = broker.o
OBJ2 = $(patsubst %,$(ODIR)/%,$(_OBJ2))

_TOBJ = singlehouse_test.o
TOBJ = $(patsubst %,$(ODIR)/%,$(_TOBJ))

$(ODIR)/%_test.o: $(TDIR)/%_test.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir $(ODIR)/house $(ODIR)/broker

socket: dir
	g++ $(LIBS) -I$(IDIR) $(SDIR)/client.cpp -o $(ODIR)/client
	g++ $(LIBS) -I$(IDIR) $(SDIR)/broker.cpp -o $(ODIR)/broker

dir:
	mkdir -p $(ODIR)

$(ODIR)/house: $(OBJ1)
	$(CC) -I$(IDIR) -o $@ $^ $(PROFILE) $(LIBS)

$(ODIR)/broker: $(OBJ2)
	$(CC) -I$(IDIR) -o $@ $^ $(PROFILE) $(LIBS)

test: dir $(OBJ) $(TOBJ)
	@$(foreach test,$(TOBJ), $(CC) -o $(patsubst %_test.o,%,$(test)) $(OBJ) $(test) -lcppunit && ./$(patsubst %_test.o,%,$(test));)

clean:
	rm -rf $(ODIR) *~ $(INCDIR)/*~

distclean: clean

rebuild: distclean all

.PHONY: clean test socket
