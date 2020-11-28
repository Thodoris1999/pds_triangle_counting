#ifndef SPARSE_GRAPH_H
#define SPARSE_GRAPH_H

class CSCGraph {
    private:
    int n;

    void triangleCountV3Serial();
    void triangleCountV3Cilk();
    void triangleCountV3Omp();

    public:
    int* row_index;
    int* col_ptr;
    int* c3;

    CSCGraph(int n, int nnz);
    ~CSCGraph();
    int at(int i, int j);
    void triangleCountV3(bool verbose, struct timespec* duration, const char* method);
    void print();
    inline int cols() const { return n; }
    inline int nnz() const { return col_ptr[n]; }
};

#endif
