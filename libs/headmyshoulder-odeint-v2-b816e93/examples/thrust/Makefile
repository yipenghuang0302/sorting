# Copyright 2011-2014 Mario Mulansky
# Copyright 2011-2012 Karsten Ahnert
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or
# copy at http://www.boost.org/LICENSE_1_0.txt)

# make sure BOOST_ROOT is pointing to your boost directory
# otherwise, set it here:
BOOST_ROOT = /Users/yipenghuang0302/Desktop/boost_1_62_0

# path to the cuda installation
CUDA_ROOT = /usr/local/cuda
#/local/cuda
# target architecture
ARCH = sm_30

NVCC = $(CUDA_ROOT)/bin/nvcc

INCLUDES += -I../../include/ -I$(BOOST_ROOT)

NVCCFLAGS = -O3 $(INCLUDES) -arch $(ARCH)

%.o : %.cu
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

% : %.o
	$(NVCC) $(NVCCFLAGS) -o $@ $<


all : phase_oscillator_chain phase_oscillator_ensemble lorenz_parameters relaxation


clean :
	-rm *~ *.o phase_oscillator_chain phase_oscillator_ensemble lorenz_parameters relaxation 
