
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include "sparse_graph.h"

#ifdef CILK
#include <cilk/cilk.h>
#endif

#ifdef OMP
#include <omp.h>
#endif

CSCGraph::CSCGraph(int n, int nnz) : n(n) {
    row_index = (int*) malloc(nnz * sizeof(int));
    col_ptr = (int*) malloc((n+1) * sizeof(int));

    c3 = (int*) malloc(this->n*sizeof(int));
    for (int i = 0; i < this->n; i++) c3[i]=0;
}

CSCGraph::~CSCGraph() {
    free(col_ptr);
    free(row_index);
    free(c3);
}

int* CSCGraph::mul(int* v) {
    int* y = (int*) malloc(n*sizeof(int));
    for (int i=0; i<n; i++) y[i]=0;

    for (int i = 0; i < n; i++) {
        for (int j = col_ptr[i]; j < col_ptr[i+1]; j++) {
            y[i] += v[row_index[j]];
        }
    }

    return y;
}

void CSCGraph::triangleCountV4Serial() {
    int nnz = col_ptr[n];
    for (int i = 0; i < n; i++) {
        for (int j = col_ptr[i]; j < col_ptr[i+1]; j++) {
            int coli_ptr = col_ptr[i];
            int colj_ptr = col_ptr[row_index[j]];
            while (coli_ptr < col_ptr[i+1] && colj_ptr < col_ptr[row_index[j]+1]) {
                int diff = row_index[colj_ptr] - row_index[coli_ptr];
                if (diff > 0) {
                    coli_ptr++;
                } else if (diff < 0) {
                    colj_ptr++;
                } else {
                    coli_ptr++;
                    colj_ptr++;
                    c3[i]++;
                }
            }
        }
        c3[i] /= 2;
    }
}

void CSCGraph::triangleCountV4Cilk(int nthreads) {
#ifdef CILK
    int nnz = col_ptr[n];
    for (int i = 0; i < n; i++) {
        for (int j = col_ptr[i]; j < col_ptr[i+1]; j++) {
            int coli_ptr = col_ptr[i];
            int colj_ptr = col_ptr[row_index[j]];
            while (coli_ptr < col_ptr[i+1] && colj_ptr < col_ptr[row_index[j]+1]) {
                int diff = row_index[colj_ptr] - row_index[coli_ptr];
                if (diff > 0) {
                    coli_ptr++;
                } else if (diff < 0) {
                    colj_ptr++;
                } else {
                    coli_ptr++;
                    colj_ptr++;
                    c3[i]++;
                }
            }
        }
    }
#endif
}
void CSCGraph::triangleCountV4Omp(int nthreads) {
#ifdef OMP
    int nnz = col_ptr[n];
    for (int i = 0; i < n; i++) {
        for (int j = col_ptr[i]; j < col_ptr[i+1]; j++) {
            int coli_ptr = col_ptr[i];
            int colj_ptr = col_ptr[row_index[j]];
            while (coli_ptr < col_ptr[i+1] && colj_ptr < col_ptr[row_index[j]+1]) {
                int diff = row_index[colj_ptr] - row_index[coli_ptr];
                if (diff > 0) {
                    coli_ptr++;
                } else if (diff < 0) {
                    colj_ptr++;
                } else {
                    coli_ptr++;
                    colj_ptr++;
                    c3[i]++;
                }
            }
        }
    }
#endif
}
void CSCGraph::triangleCountV4Pthreads(int nthreads) {
#ifdef PTHREADS
    int nnz = col_ptr[n];
    for (int i = 0; i < n; i++) {
        for (int j = col_ptr[i]; j < col_ptr[i+1]; j++) {
            int coli_ptr = col_ptr[i];
            int colj_ptr = col_ptr[row_index[j]];
            while (coli_ptr < col_ptr[i+1] && colj_ptr < col_ptr[row_index[j]+1]) {
                int diff = row_index[colj_ptr] - row_index[coli_ptr];
                if (diff > 0) {
                    coli_ptr++;
                } else if (diff < 0) {
                    colj_ptr++;
                } else {
                    coli_ptr++;
                    colj_ptr++;
                    c3[i]++;
                }
            }
        }
    }
#endif
}

// random access Θ(nnz/n)
int CSCGraph::at(int i, int j) {
    // column slice
    for (int k = col_ptr[j]; k < col_ptr[j+1]; k++) {
        if (row_index[k] == i) return 1;
    }
    return 0;
}

void CSCGraph::triangleCountV3Serial() {
    int nnz = col_ptr[n];
    for (int i = 0; i < n; i++) {
        for (int j = col_ptr[i]; j < col_ptr[i+1]; j++)  {
            if (row_index[j] <= i) continue; // only evaluate i < j < k
            for (int k = j+1; k < col_ptr[i+1]; k++) {
                if (at(row_index[k], row_index[j]) == 1) {
                    c3[i]++;
                    c3[row_index[j]]++;
                    c3[row_index[k]]++;
                }
            }
        }
    }
}

void CSCGraph::triangleCountV3Cilk(int nthreads) {
#ifdef CILK
    __cilkrts_set_param("nworkers", itoa(nthreads))
    int nnz = col_ptr[n];
    cilk_for (int i = 0; i < n; i++) {
        cilk_for (int j = col_ptr[i]; j < col_ptr[i+1]; j++)  {
            if (row_index[j] <= i) continue; // only evaluate i < j < k
            for (int k = j+1; k < col_ptr[i+1]; k++) {
                if (at(row_index[k], row_index[j]) == 1) {
                    c3[i]++;
                    c3[row_index[j]]++;
                    c3[row_index[k]]++;
                }
            }
	}
    }
#endif
}

void CSCGraph::triangleCountV3Omp(int nthreads) {
#ifdef OMP
    int nnz = col_ptr[n];
    omp_set_num_threads(nthreads);
    #pragma omp parallel 
    {
    int id = omp_get_thread_num();
    int nthrds = omp_get_num_threads();
    for (int i = id; i < n; i+=nthrds) {
        for (int j = col_ptr[i]; j < col_ptr[i+1]; j++)  {
            if (row_index[j] <= i) continue; // only evaluate i < j < k
            for (int k = j+1; k < col_ptr[i+1]; k++) {
                if (at(row_index[k], row_index[j]) == 1) {
                    #pragma omp critical
                    {
                    c3[i]++;
                    c3[row_index[j]]++;
                    c3[row_index[k]]++;
                    }
                }
            }
	}
    }
    }
#endif
}

void CSCGraph::triangleCountV3(bool verbose, struct timespec* duration, const char* method, int nthreads) {
    struct timespec ts_start;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    if (verbose) printf("start stamp: %ldsec, %ldnsec\n", ts_start.tv_sec, ts_start.tv_nsec);

    if (strcmp(method, "cilk") == 0) {
        if (nthreads == 0)
            triangleCountV3Cilk(8);
        else
            triangleCountV3Cilk(nthreads);
    } else if (strcmp(method, "omp") == 0) {
        if (nthreads == 0)
            triangleCountV3Omp(8);
        else
            triangleCountV3Omp(nthreads);
    } else {
        triangleCountV3Serial();
    }

    struct timespec ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    if (verbose) printf("end stamp: %ldsec, %ldnsec\n", ts_end.tv_sec, ts_end.tv_nsec);
    duration->tv_sec = ts_end.tv_sec - ts_start.tv_sec;
    duration->tv_nsec = ts_end.tv_nsec - ts_start.tv_nsec;
    while (duration->tv_nsec > 1000000000) {
        duration->tv_sec++;
        duration->tv_nsec -= 1000000000;
    }
    while (duration->tv_nsec < 0) {
        duration->tv_sec--;
        duration->tv_nsec += 1000000000;
    }

    if (verbose) {
        printf("c3: ");
        if (this->n < 10000) {
            for (int i = 0; i < this->n; i++) {
                printf("%d ", c3[i]);
            }
        } else {
            printf("c3 vector too big to preview\n");
        }
        printf("\n");
        double dur_d = duration->tv_sec + duration->tv_nsec/1000000000.0;
        printf("V3 duration: %lf seconds\n", dur_d);
    }
}

void CSCGraph::triangleCountV4(bool verbose, struct timespec* duration, const char* method, int nthreads) {
    struct timespec ts_start;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    if (verbose) printf("start stamp: %ldsec, %ldnsec\n", ts_start.tv_sec, ts_start.tv_nsec);

    if (strcmp(method, "cilk") == 0) {
        if (nthreads == 0)
            triangleCountV4Cilk(8);
        else
            triangleCountV4Cilk(nthreads);
    } else if (strcmp(method, "omp") == 0) {
        if (nthreads == 0)
            triangleCountV4Omp(8);
        else
            triangleCountV4Omp(nthreads);
    } else if (strcmp(method, "pthreads") == 0) {
        if (nthreads == 0)
            triangleCountV4Pthreads(8);
        else
            triangleCountV4Pthreads(nthreads);
    } else {
        triangleCountV4Serial();
    }

    struct timespec ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    if (verbose) printf("end stamp: %ldsec, %ldnsec\n", ts_end.tv_sec, ts_end.tv_nsec);
    duration->tv_sec = ts_end.tv_sec - ts_start.tv_sec;
    duration->tv_nsec = ts_end.tv_nsec - ts_start.tv_nsec;
    while (duration->tv_nsec > 1000000000) {
        duration->tv_sec++;
        duration->tv_nsec -= 1000000000;
    }
    while (duration->tv_nsec < 0) {
        duration->tv_sec--;
        duration->tv_nsec += 1000000000;
    }

    if (verbose) {
        printf("c3: ");
        if (this->n < 10000) {
            for (int i = 0; i < this->n; i++) {
                printf("%d ", c3[i]);
            }
        } else {
            printf("c3 vector too big to preview\n");
        }
        printf("\n");
        double dur_d = duration->tv_sec + duration->tv_nsec/1000000000.0;
        printf("V4 duration: %lf seconds\n", dur_d);
    }
}

void CSCGraph::print() {
    int nnz = col_ptr[n];
    if (nnz > 10000) {
        printf("matrix too big to preview\n");
        return;
    }
    printf("vals: ");
    for (int i = 0; i < nnz; i++)
        printf("%d ", 1);
    printf("\n");
    printf("row indices: ");
    for (int i = 0; i < nnz; i++)
        printf("%d ", row_index[i]);
    printf("\n");
    printf("col ptr: ");
    for (int i = 0; i < n+1; i++)
        printf("%d ", col_ptr[i]);
    printf("\n");
}
