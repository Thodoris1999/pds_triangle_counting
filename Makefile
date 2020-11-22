CC=g++
DEFINITIONS=GRAPHVIZ 
CFLAGS=`pkg-config libcgraph --cflags` -Wall -O3 -DGRAPHVIZ -DWITH_CGRAPH
LDFLAGS=`pkg-config libcgraph --libs`

BINS_DIR=bin
C_SOURCES = src/mmio.c
CPP_SOURCES = src/sparse_graph.cpp
SOURCES = $(C_SOURCES) $(CPP_SOURCES)

default: all

.PHONY: clean

bin:
	mkdir -p $@

main: | bin
	$(CC) $(CFLAGS) -o $(BINS_DIR)/$@ $(SOURCES) src/main.cpp $(LDFLAGS)

v3: | bin
	$(CC) $(CFLAGS) -o $(BINS_DIR)/$@ $(SOURCES) src/v3.cpp $(LDFLAGS)

all: main v3

clean:
	rm -rf $(BINS_DIR)
