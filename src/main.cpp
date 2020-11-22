#include "utils.h"
#include <ctime>

void triangle_countv1(int** A, int nv, bool verbose, struct timespec* duration) {
    struct timespec ts_start;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);

    int* c3 = (int*) malloc(nv*sizeof(int));
    for (int i = 0; i < nv; i++) c3[i]=0;

    for (int i = 0; i < nv; i++) {
        for (int j = 0; j < nv; j++) {
            for (int k = 0; k < nv; k++) {
                if (i == j && j == k)
                    continue;
                if (A[i][j] == A[i][k] && A[i][j] == A[j][k] && A[i][j] == 1) {
                    c3[i]++;
                    c3[j]++;
                    c3[k]++;
                }
            }
        }
    }

    struct timespec ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
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
        for (int i = 0; i < nv; i++) {
            printf("%d ", c3[i]/6);
        }
        printf("\n");
        double dur_d = duration->tv_sec + duration->tv_nsec/1000000000.0;
        printf("V1 duration: %lf seconds\n", dur_d);
    }
    free(c3);
}

void triangle_countv2(int** A, int nv, bool verbose, struct timespec* duration) {
    struct timespec ts_start;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);

    int* c3 = (int*) malloc(nv*sizeof(int));
    for (int i = 0; i < nv; i++) c3[i]=0;

    for (int i = 0; i < nv; i++) {
        for (int j = i+1; j < nv; j++) {
            for (int k = j+1; k < nv; k++) {
                if (A[i][j] == A[i][k] && A[i][j] == A[j][k] && A[i][j] == 1) {
                    c3[i]++;
                    c3[j]++;
                    c3[k]++;
                }
            }
        }
    }

    struct timespec ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
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
        for (int i = 0; i < nv; i++) {
            printf("%d ", c3[i]);
        }
        printf("\n");
        double dur_d = duration->tv_sec + duration->tv_nsec/1000000000.0;
        printf("V2 duration: %lf seconds\n", dur_d);
    }
    free(c3);
}

void testv1(int max_size, char* outfile, bool verbose) {
    FILE *fp = fopen(outfile, "w+");
    struct timespec duration;
    for (int i = 4; i < max_size; i++) {
        int** g = utils::create_rand_graph(i, i*i/3);
        triangle_countv1(g, i, verbose, &duration);
        double dur_d = duration.tv_sec + duration.tv_nsec/1000000000.0;
        fprintf(fp, "%d %lf\n", i, dur_d);
        utils::free_graph(g, i);
    }
    fclose(fp);
}

void testv2(int max_size, char* outfile, bool verbose) {
    FILE *fp = fopen(outfile, "w+");
    struct timespec duration;
    for (int i = 4; i < max_size; i++) {
        int** g = utils::create_rand_graph(i, i*i/3);
        triangle_countv2(g, i, verbose, &duration);
        double dur_d = duration.tv_sec + duration.tv_nsec/1000000000.0;
        fprintf(fp, "%d %lf\n", i, dur_d);
        utils::free_graph(g, i);
    }
    fclose(fp);
}

int main(int argc, char** argv) {
    int nv = 15;
    int ne = 20;
    int** g = utils::create_rand_graph(nv, ne);

    utils::print_mat(g, nv, nv);
    utils::viz_mat(g, nv);
    struct timespec duration;
    triangle_countv1(g, nv, true, &duration);
    triangle_countv2(g, nv, true, &duration);

    //testv1(200, "timesv1.txt", 0);
    //testv2(200, "timesv2.txt", 0);

    if (argc > 1) {
        CSCGraph g = utils::parseMMGraph(argv[1]);
        g.print();
    }

    utils::free_graph(g, nv);
    return 0;
}
