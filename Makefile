# makefile (use -pg for profiling)

IDIR = include
CC = g++
DEBUG = -g
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
_OBJ1 = mc.o query2.o
OBJ1 = $(patsubst %,$(ODIR)/%,$(_OBJ1))

_OBJ2 = broker.o
OBJ2 = $(patsubst %,$(ODIR)/%,$(_OBJ2))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir $(ODIR)/query2 $(ODIR)/broker

dir:
	mkdir -p $(ODIR)

$(ODIR)/query2: $(OBJ1)
	$(CC) -I$(IDIR) -o $@ $^ $(PROFILE) $(LIBS)

$(ODIR)/broker: $(OBJ2)
	$(CC) -I$(IDIR) -o $@ $^ $(PROFILE) $(LIBS)

clean:
	rm -rf $(ODIR) *~ $(INCDIR)/*~

distclean: clean

rebuild: distclean all

.PHONY: clean
