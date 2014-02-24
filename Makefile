# makefile (use -pg for profiling)

IDIR = include
CC = g++
DEBUG = -g
PROFILE =
CFLAGS = -Wall -c $(DEBUG) -I$(IDIR) $(PROFILE)

SDIR = src
ODIR = bin
TDIR = test
LIBS = -lm -lpthread -std=c++0x

# header files => .cpp files
_DEPS = mc.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
_OBJ = mc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TOBJ = singlehouse_test.o
TOBJ = $(patsubst %,$(ODIR)/%,$(_TOBJ))

$(ODIR)/%_test.o: $(TDIR)/%_test.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir $(ODIR)/house

socket: dir
	g++ $(LIBS) -I$(IDIR) $(SDIR)/client.cpp -o $(ODIR)/client
	g++ $(LIBS) -I$(IDIR) $(SDIR)/server.cpp -o $(ODIR)/server

dir:
	mkdir -p $(ODIR)

$(ODIR)/house: $(OBJ)
	$(CC) -I$(IDIR) -o $@ $^ $(SDIR)/house.cpp $(PROFILE) $(LIBS)

test: dir $(OBJ) $(TOBJ)
	@$(foreach test,$(TOBJ), $(CC) -o $(patsubst %_test.o,%,$(test)) $(OBJ) $(test) -lcppunit && ./$(patsubst %_test.o,%,$(test));)

clean:
	rm -rf $(ODIR) *~ $(INCDIR)/*~

distclean: clean

rebuild: distclean all

.PHONY: clean
