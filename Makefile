CC=gcc
CPPC=g++
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang

BINS_DIR=bin
MMIO_LIB=$(BINS_DIR)/mmio.o
C_SOURCES = src/mmio.c
CPP_SOURCES = src/sparse_graph.cpp
SOURCES = $(C_SOURCES) $(CPP_SOURCES)
GRAPHVIZ_DEFS=-DGRAPHVIZ -DWITH_CGRAPH

CFLAGS=`pkg-config libcgraph --cflags` -Wall -O3 $(GRAPHVIZ_DEFS)
CILKFLAGS=-Wall -O3 -fcilkplus -DCILK
LDFLAGS=`pkg-config libcgraph --libs` $(MMIO_LIB)

default: all

.PHONY: clean

bin:
	mkdir -p $@

mmio: | bin 
	$(CC) -c $(CFLAGS) -o $(MMIO_LIB) $(C_SOURCES)

main: | bin mmio
	$(CPPC) $(CFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/main.cpp $(LDFLAGS)

v3: | bin mmio
	$(CPPC) $(CFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/v3.cpp $(LDFLAGS)

v3_cilk: | bin mmio
	$(CILKCC) $(CILKFLAGS) -o $(BINS_DIR)/$@ $(CPP_SOURCES) src/v3_cilk.cpp $(LDFLAGS) -lstdc++

all: main v3 v3_cilk

clean:
	rm -rf $(BINS_DIR)
