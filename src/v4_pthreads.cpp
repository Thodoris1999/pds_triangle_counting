#include "utils.h"
#include "sparse_graph.h"

#include <dirent.h>
#include <cstring>

int main(int argc, char** argv) {
    if (argc > 1) {
        CSCGraph g = utils::parseMMGraphFull(argv[1]);
        g.print();

        struct timespec duration;
        g.triangleCountV4(1, &duration, "pthreads");

        if (argc > 2 && strcmp(argv[2], "--debug") == 0) {
            CSCGraph g2 = utils::parseMMGraph(argv[1]);
            g2.triangleCountV3(0, &duration, "serial");
            int c3_wrong_count = 0;
            for (int i = 0; i < g2.cols(); i++) {
                if (g2.c3[i] != g.c3[i])
                    c3_wrong_count++;
            }
            printf("V4 pthreads and V3 c3 differed in %d elements\n", c3_wrong_count);
        } else if (argc > 2 && strcmp(argv[2], "--test") == 0) {
            FILE *fp = fopen("v4_pthreads_bmark.txt", "w+");
            for (int i = 1; i <= 16; i++) {
                g.triangleCountV4(1, &duration, "pthreads", i); 
                double dur_d = duration.tv_sec + duration.tv_nsec/1000000000.0;
                fprintf(fp, "%lf\n", dur_d);
            }

            fclose(fp);
        }

        return 0;
    }

    DIR* d = opendir("data");
    struct dirent *dir;
    if (d) {
        FILE *fp = fopen("v4_pthreads_times.txt", "w+");
        while ((dir = readdir(d)) != nullptr) {
            char* rchr = strrchr(dir->d_name, '.');
            if (rchr == nullptr || strcmp(rchr, ".mtx") != 0) {
                printf("skipping %s\n", dir->d_name);
                continue; // not matrix market file
            }
            char* path = (char*) malloc((strlen(dir->d_name)+strlen("data/")+2) * sizeof(char));
            strcpy(path, "data/");
            strcat(path, dir->d_name);
            CSCGraph g = utils::parseMMGraphFull(path);
            free(path);
            g.print();

            struct timespec duration;
            g.triangleCountV4(1, &duration, "pthreads");
            double dur_d = duration.tv_sec + duration.tv_nsec/1000000000.0;
            fprintf(fp, "%s %d %d %lf\n", dir->d_name, g.cols(), g.nnz(), dur_d);
        }
        fclose(fp);
        closedir(d);
    }

    return 0;
}
