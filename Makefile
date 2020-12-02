CC=gcc
CPPC=g++
CILKCC=clang

BINS_DIR=bin
MMIO_LIB=$(BINS_DIR)/mmio.o
C_SOURCES = src/mmio.c
CPP_SOURCES = src/sparse_graph.cpp
SOURCES = $(C_SOURCES) $(CPP_SOURCES)
GRAPHVIZ_DEFS=-DGRAPHVIZ -DWITH_CGRAPH

CFLAGS=`pkg-config libcgraph --cflags` -std=c++0x -Wall -O3 #$(GRAPHVIZ_DEFS)
CILKFLAGS=-Wall -O3 -fcilkplus -DCILK
OMPFLAGS=-std=c++0x -Wall -O3 -fopenmp -DOMP
PTHREADSFLAGS=-std=c++0x -Wall -O3 -pthread -DPTHREADS
LDFLAGS=`pkg-config libcgraph --libs` $(MMIO_LIB)

default: all

.PHONY: clean

bin:
	mkdir -p $@

data:
	./get_data.bash

mmio: | bin 
	$(CC) -c $(CFLAGS) -o $(MMIO_LIB) $(C_SOURCES)

main: | bin mmio
	$(CPPC) $(CFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/main.cpp $(LDFLAGS)

v3: | bin mmio
	$(CPPC) $(CFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/v3.cpp $(LDFLAGS)

v3_cilk: | bin
	$(CILKCC) $(CILKFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/mmio.c src/v3_cilk.cpp -lstdc++

v3_omp: | bin mmio
	$(CPPC) $(OMPFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/v3_omp.cpp $(LDFLAGS) -lstdc++

v4: | bin mmio
	$(CPPC) $(CFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/v4.cpp $(LDFLAGS)

v4_cilk: | bin
	$(CILKCC) $(CILKFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/mmio.c src/v4_cilk.cpp -lstdc++

v4_omp: | bin mmio
	$(CPPC) $(OMPFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/v4_omp.cpp $(LDFLAGS) -lstdc++

v4_pthreads: | bin mmio
	$(CPPC) $(PTHREADSFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/v4_pthreads.cpp $(LDFLAGS)

all: main v3 v3_cilk v3_omp v4 v4_cilk v4_omp v4_pthreads

clean:
	rm -rf $(BINS_DIR)
