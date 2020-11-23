
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include "sparse_graph.h"

#ifdef CILK
#include <cilk/cilk.h>
#endif

CSCGraph::CSCGraph(int n, int nnz) : n(n) {
    row_index = (int*) malloc(nnz * sizeof(int));
    col_ptr = (int*) malloc((n+1) * sizeof(int));
}

CSCGraph::~CSCGraph() {
    free(col_ptr);
    free(row_index);
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
    c3 = (int*) malloc(this->n*sizeof(int));
    for (int i = 0; i < this->n; i++) c3[i]=0;

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

void CSCGraph::triangleCountV3Cilk() {
#ifdef CILK
    c3 = (int*) malloc(this->n*sizeof(int));
    cilk_for (int i = 0; i < this->n; i++) c3[i]=0;

    int nnz = col_ptr[n];
    cilk_for (int i = 0; i < n; i++) {
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
#endif
}

void CSCGraph::triangleCountV3(bool verbose, struct timespec* duration, const char* method) {
    struct timespec ts_start;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    if (verbose) printf("start stamp: %ldsec, %ldnsec\n", ts_start.tv_sec, ts_start.tv_nsec);

    if (strcmp(method, "cilk") == 0) {
        triangleCountV3Cilk();
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
    free(c3);
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
