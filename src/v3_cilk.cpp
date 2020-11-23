#include "utils.h"
#include "sparse_graph.h"

int main(int argc, char** argv) {
    if (argc > 1) {
        CSCGraph g = utils::parseMMGraph(argv[1]);
        g.print();

        struct timespec duration;
        g.triangleCountV3(1, &duration, "cilk");
    }   
    return 0;
}
