CC=g++
DEFINITIONS=GRAPHVIZ 
CFLAGS=`pkg-config libcgraph --cflags` -Wall -O3 -DGRAPHVIZ -DWITH_CGRAPH
LDFLAGS=`pkg-config libcgraph --libs`

BINS_DIR=bin
SRC_DIR=src

default: all

.PHONY: clean

bin:
	mkdir -p $@

main: | bin
	$(CC) $(CFLAGS) -o $(BINS_DIR)/$@ $(SRC_DIR)/main.cpp $(LDFLAGS)

all: main

clean:
	rm -rf $(BINS_DIR)
