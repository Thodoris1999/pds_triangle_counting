#ifndef UTILS_H
#define UTILS_H

#include <vector>

#include <cstdlib>
#include <cassert>
#include <cstdio>

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
