# make sure BOOST_ROOT is pointing to your boost directory
# otherwise, set it here:
BOOST_ROOT = /Developer/usr/include/

# path to the cuda installation
CUDA_ROOT = /usr/local/cuda

# target architecture
ARCH = sm_30

NVCC = nvcc

INCLUDES += -I$(BOOST_ROOT) -I/home/yipeng/boost_1_62_0/

NVCCFLAGS = -arch $(ARCH) -O3 $(INCLUDES) -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_TBB
# -Xcompiler -fopenmp -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP
# -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_CUDA

ODIR=obj

LIBS= -lm -ltbb
# -L/home/yipeng/lib -lpapi -lgomp -lcudart -L/home/yipeng/cuda-8.0/extras/CUPTI/lib64 -L/usr/local/cuda-8.0/extras/CUPTI/lib64 -lcupti -lcuda

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
