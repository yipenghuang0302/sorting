BOOST_ROOT = /Developer/usr/include/

CC=gcc
INCLUDES += -I$(BOOST_ROOT) -I/home/yipeng/include/
CFLAGS=-Wall -O3 $(INCLUDES) -pg

ODIR=obj

LIBS=-lm -lstdc++

_DEPS = hamiltonian.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = sort.o qr.o bracket.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: test

$(ODIR)/%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ test $(INCDIR)/*~ 