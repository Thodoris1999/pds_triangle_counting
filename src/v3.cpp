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
            CSCGraph g = utils::parseMMGraph(dir->d_name);
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
