# make sure BOOST_ROOT is pointing to your boost directory
# otherwise, set it here:
BOOST_ROOT = /Users/yipenghuang0302/Desktop/boost_1_62_0

# path to the cuda installation
CUDA_ROOT = /usr/local/cuda

# target architecture
ARCH = sm_30

NVCC = $(CUDA_ROOT)/bin/nvcc

INCLUDES += -I$(BOOST_ROOT)

NVCCFLAGS = -O3 $(INCLUDES) --use_fast_math
# --warn-on-local-memory-usage	
# -arch $(ARCH)

ODIR=obj

LIBS=-lm -lstdc++

_DEPS = hamiltonian.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ =  main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: test

$(ODIR)/%.o: %.cu
	$(NVCC) -c -o $@ $< $(NVCCFLAGS) $(LIBS)

test: $(OBJ)
	$(NVCC) -o $@ $^ $(NVCCFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ test $(INCDIR)/*~ 
