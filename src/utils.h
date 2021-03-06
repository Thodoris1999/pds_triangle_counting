#ifndef UTILS_H
#define UTILS_H

#include <vector>

#include <cstdlib>
#include <cassert>
#include <cstdio>

#include "sparse_graph.h"
extern "C" {
#include "mmio.h"
}

#ifdef GRAPHVIZ
#include <gvc.h>
#include <cgraph.h>
#endif

namespace utils {
inline void print_mat(int** A, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
}

#ifdef GRAPHVIZ
inline void viz_mat(int** A, int nv) {
    if (nv > 80) {
        printf("Matrix too big to visualize\n");
    }
    Agraph_t *g = agopen("graph", Agundirected, nullptr);
    Agnode_t** node_arr = (Agnode_t**) malloc(nv * sizeof(Agnode_t*));
    for (int i = 0; i < nv; i++) {
        char v_name[100];
        sprintf(v_name, "%d", i);
        Agnode_t *n = agnode(g, v_name, 1);
        node_arr[i] = n;
    }

    for (int i = 0; i < nv; i++) {
        for (int j = i+1; j < nv; j++) {
            if (A[i][j] == 1) {
                char e_name[100];
                sprintf(e_name, "%d_%d", i, j);
                Agedge_t *e = agedge(g, node_arr[i], node_arr[j], e_name, 1);
            }
        }
    }

    FILE* fp = fopen("out.dot", "w+");
    agwrite(g, fp);
    fclose(fp);
    free(node_arr);
    agclose(g);
}
#endif

inline CSCGraph parseMMGraph(char* filename) {
    printf("Parsing %s\n", filename);
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
    int M, N, nz;
    MM_typecode t;
    int res;
    res = mm_read_banner(fp, &t);
    if (res) {
        printf("Error reading banner: %d\n", res);
        exit(res);
    }
    if (!mm_is_sparse(t)) {
        printf("Error, matrix is not sparse\n");
        exit(2);
    }
    bool pattern = true;
    if (!mm_is_pattern(t)) {
        printf("Warning: matrix is not pattern\n");
        pattern = false;
    }
    if (!mm_is_symmetric(t)) {
        printf("Error, matrix is not symmetric\n");
        exit(4);
    }
    res = mm_read_mtx_crd_size(fp, &M, &N, &nz);
    if (res) {
        printf("Error reading size information: %d\n", res);
        exit(res);
    }
    if (M != N) {
        printf("Error, matrix is not square\n");
        exit(5);
    }

    CSCGraph g(N, nz);
    bool wrn_nzoo = true;
    g.col_ptr[0] = 0;
    int col_idx;
    printf("Matrix market file %s valid, parsing...\n", filename);
    for (int i=0; i<nz; i++)
    {
        if (!pattern) {
            double val;
            fscanf(fp, "%d %d %lf\n", &g.row_index[i], &col_idx, &val);
            if (val != 0 && val != 1) {
                if (wrn_nzoo) {
                    printf("Warning: non-zero-or-one element %lf, will be interpreted as 1, ", val);
                    printf("this warning is only shown once\n");
                    wrn_nzoo = false;
                }
            }
        } else {
            fscanf(fp, "%d %d\n", &g.row_index[i], &col_idx);
        }
        g.row_index[i]--;  /* adjust from 1-based to 0-based */
        g.col_ptr[col_idx]++; // col_ptr temporarily holds the number of nz in the previous column
    }
    printf("Matrix market file %s parsed successfully\n", filename);
    printf("N: %d, lower triangular NZ: %d\n", N, nz);

    // cumsum
    for (int i = 0; i < N; i++) {
        g.col_ptr[i+1] += g.col_ptr[i];
    }
    
    assert(g.col_ptr[N] == nz);

    fclose(fp);
    printf("Sparse matrix from file %s created successfully\n", filename);
    return g;
}

// returns both the lower and upper triangular part
inline CSCGraph parseMMGraphFull(char* filename) {
    printf("Parsing %s\n", filename);
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
    int M, N, nz;
    MM_typecode t;
    int res;
    res = mm_read_banner(fp, &t);
    if (res) {
        printf("Error reading banner: %d\n", res);
        exit(res);
    }
    if (!mm_is_sparse(t)) {
        printf("Error, matrix is not sparse\n");
        exit(2);
    }
    bool pattern = true;
    if (!mm_is_pattern(t)) {
        printf("Warning: matrix is not pattern\n");
        pattern = false;
    }
    if (!mm_is_symmetric(t)) {
        printf("Error, matrix is not symmetric\n");
        exit(4);
    }
    res = mm_read_mtx_crd_size(fp, &M, &N, &nz);
    if (res) {
        printf("Error reading size information: %d\n", res);
        exit(res);
    }
    if (M != N) {
        printf("Error, matrix is not square\n");
        exit(5);
    }

    CSCGraph g(N);
    bool wrn_nzoo = true;
    g.col_ptr[0] = 0;
    int* col_index = (int*) malloc(nz*sizeof(int));
    int* row_index = (int*) malloc(nz*sizeof(int));
    int n_extra = 0;
    printf("Matrix market file %s valid, parsing...\n", filename);
    for (int i=0; i<nz; i++)
    {
        if (!pattern) {
            double val;
            fscanf(fp, "%d %d %lf\n", &row_index[i+n_extra], &col_index[i+n_extra], &val);
            if (val != 0 && val != 1) {
                if (wrn_nzoo) {
                    printf("Warning: non-zero-or-one element %lf, will be interpreted as 1, ", val);
                    printf("this warning is only shown once\n");
                    wrn_nzoo = false;
                }
            }
        } else {
            fscanf(fp, "%d %d\n", &row_index[i+n_extra], &col_index[i+n_extra]);
        }

        row_index[i+n_extra]--;  /* adjust from 1-based to 0-based */
        col_index[i+n_extra]--;
        g.col_ptr[col_index[i+n_extra]+1]++; // col_ptr temporarily holds the number of nz in the previous column

        if (row_index[i+n_extra] == col_index[i+n_extra]) { // ignore diagonal elements
            g.col_ptr[col_index[i+n_extra]+1]--;
            n_extra--;
        } else if (row_index[i+n_extra] != col_index[i+n_extra]) { // non-diagonal entry
            n_extra++;
            row_index = (int*) realloc(row_index, (nz+n_extra)*sizeof(int));
            col_index = (int*) realloc(col_index, (nz+n_extra)*sizeof(int));

            row_index[i+n_extra] = col_index[i+n_extra-1];
            col_index[i+n_extra] = row_index[i+n_extra-1];
            
            g.col_ptr[col_index[i+n_extra]+1]++;
        }
    }
    printf("Matrix market file %s parsed successfully\n", filename);
    printf("N: %d, lower triangular NZ: %d, total nz: %d\n", N, nz, nz+n_extra);

    // cumsum
    for (int i = 0; i < N; i++) {
        g.col_ptr[i+1] += g.col_ptr[i];
    }

    // move row indices so row_index are in column major order
    // g.col_index holds the position in g.row_index that the next row_index of the i-th column should be placed in
    g.row_index = (int*) malloc((nz+n_extra)*sizeof(int));
    for (int i = 0; i < nz+n_extra; i++) {
         int dst = g.col_ptr[col_index[i]];
         g.row_index[dst] = row_index[i];
         g.col_ptr[col_index[i]]++;
    }

    // undo changes in col_index
    int prev = 0;
    for (int i = 0; i < N; i++) {
        int tmp = g.col_ptr[i];
        g.col_ptr[i] = prev;
        prev = tmp;
    }

    assert(g.col_ptr[N] == nz+n_extra);

    free(col_index);
    free(row_index);
    fclose(fp);
    printf("Sparse matrix from file %s created successfully\n", filename);
    return g;
}

inline int** create_rand_graph(int nv, int ne) {
    assert(ne <= nv*(nv-1)/2);

    int** A = (int**) malloc(nv * sizeof(int*));
    for (int i = 0; i < nv; i++) {
        A[i] = (int*) malloc(nv * sizeof(int));
        for (int j = 0; j < nv; j++) {
            A[i][j] = 0;
        }
    }

    std::vector<std::vector<int>> perms;
    for (int i = 0; i < nv; i++) {
        for (int j = i+1; j < nv; j++) {
            std::vector<int> pair {i,j};
            perms.emplace_back(pair);
        }
    }
    
    for (int i = 0; i < ne; i++) {
        int k = rand() % perms.size();
        A[perms[k][0]][perms[k][1]] = 1;
        A[perms[k][1]][perms[k][0]] = 1;
        perms.erase(perms.begin()+k);
    }

    return A;
}

inline void free_graph(int** A, int nv) {
    for (int i = 0; i < nv; i++) {
        free(A[i]);
    }
    free(A);
}
} // ns

#endif
