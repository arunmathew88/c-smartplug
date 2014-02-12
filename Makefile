# makefile (use -pg for profiling)

IDIR = include
CC = g++
DEBUG = -g
PROFILE =
CFLAGS = -Wall -c $(DEBUG) -I$(IDIR) $(PROFILE)

SDIR = src
ODIR = bin
LIBS = -lm -lpthread

# header files => .cpp files
_DEPS = mc.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
_OBJ = mc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir $(ODIR)/house_process

dir:
	mkdir -p $(ODIR)

$(ODIR)/house_process: $(OBJ)
	$(CC) -I$(IDIR) -o $@ $^ $(SDIR)/house_process.cpp $(PROFILE) $(LIBS)

clean:
	rm -rf $(ODIR) *~ $(INCDIR)/*~

distclean: clean

rebuild: distclean all

.PHONY: clean
