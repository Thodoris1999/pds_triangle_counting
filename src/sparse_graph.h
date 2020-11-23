#ifndef SPARSE_GRAPH_H
#define SPARSE_GRAPH_H

class CSCGraph {
    private:
    int n;
    int* c3;

    void triangleCountV3Serial();
    void triangleCountV3Cilk();

    public:
    int* row_index;
    int* col_ptr;

    CSCGraph(int n, int nnz);
    ~CSCGraph();
    int at(int i, int j);
    void triangleCountV3(bool verbose, struct timespec* duration, const char* method);
    void print();
    inline int cols() const { return n; }
};

#endif
