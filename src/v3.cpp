#include "utils.h"
#include "sparse_graph.h"

#include <dirent.h>

int main(int argc, char** argv) {
    if (argc > 1) {
        CSCGraph g = utils::parseMMGraph(argv[1]);
        g.print();

        struct timespec duration;
        g.triangleCountV3(1, &duration, "serial");
        return 0;
    }

    DIR* d = opendir("data");
    struct dirent *dir;
    if (d) {
        FILE *fp = fopen("v3_serial_times.txt", "w+");
        while ((dir = readdir(d)) != nullptr) {
            char* rchr = strrchr(dir->d_name, '.');
            if (rchr == nullptr || strcmp(rchr, ".mtx") != 0) {
                printf("skipping %s\n", dir->d_name);
                continue; // not matrix market file
            }
            char* path = (char*) malloc((strlen(dir->d_name)+strlen("data/")+2) * sizeof(char));
            strcpy(path, "data/");
            strcat(path, dir->d_name);
            CSCGraph g = utils::parseMMGraph(path);
            free(path);
            g.print();

            struct timespec duration;
            g.triangleCountV3(1, &duration, "serial");
            double dur_d = duration.tv_sec + duration.tv_nsec/1000000000.0;
            fprintf(fp, "%s %d %d %lf\n", dir->d_name, g.cols(), g.nnz(), dur_d);
        }
        fclose(fp);
        closedir(d);
    }

    return 0;
}
