#include "utils.h"
#include "sparse_graph.h"

#include "dirent.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char** argv) {
    if (argc > 1) {
        CSCGraph g = utils::parseMMGraph(argv[1]);
        g.print();

        struct timespec duration;
        g.triangleCountV3(1, &duration, "cilk");

        if (argc > 2 && strcmp(argv[2], "--debug") == 0) {
            CSCGraph g2 = utils::parseMMGraph(argv[1]);
            g2.triangleCountV3(0, &duration, "serial");
            int c3_wrong_count = 0;
            for (int i = 0; i < g2.cols(); i++) {
                if (g2.c3[i] != g.c3[i])
                    c3_wrong_count++;
            }
            printf("Serial and parallel c3 differed in %d elements\n", c3_wrong_count);
        }
    }   

    DIR* d = opendir("data");
    struct dirent *dir;
    if (d) {
        FILE *fp = fopen("v3_cilk_times.txt", "w+");
        while ((dir = readdir(d)) != nullptr) {
            CSCGraph g = utils::parseMMGraph(dir->d_name);
            g.print();

            struct timespec duration;
            g.triangleCountV3(1, &duration, "cilk");
            double dur_d = duration.tv_sec + duration.tv_nsec/1000000000.0;
            fprintf(fp, "%s %d %d %lf\n", dir->d_name, g.cols(), g.nnz(), dur_d);
        }
        fclose(fp);
        closedir(d);
    }

    return 0;
}

