#ifndef SPARSE_GRAPH_H
#define SPARSE_GRAPH_H

class CSCGraph {
    private:
    int n;

    void triangleCountV3Serial();
    void triangleCountV3Cilk(int nthreads);
    void triangleCountV3Omp(int nthreads);

    void triangleCountV4Serial();
    void triangleCountV4Cilk(int nthreads);
    void triangleCountV4Omp(int nthreads);
    void triangleCountV4Pthreads(int nthreads);

    public:
    int* row_index;
    int* col_ptr;
    int* c3;

    // only allocates col_ptr and c3
    CSCGraph(int n);
    CSCGraph(int n, int nnz);
    ~CSCGraph();
    int at(int i, int j);
    int* mul(int* v);
    void triangleCountV3(bool verbose, struct timespec* duration, const char* method, int nthreads=0);
    void triangleCountV4(bool verbose, struct timespec* duration, const char* method, int nthreads=0);
    void print();
    inline int cols() const { return n; }
    inline int nnz() const { return col_ptr[n]; }
};

#endif
