#include <stdlib.h>
#include <stdio.h>
#include "dfs.h"
#include "kahn.h"

#define MAX_NUM_VERTICES 32768
#define NUM_RUN_AVERAGE 3

int main() {
    char filename[32];

    for(MaxDensity density = DENSITY_25; density <= DENSITY_75; density++) {
        printf("Testing graph density option = %d\n", density);
        sprintf(filename, "stats_density_%d.txt", density);
        FILE *fp = fopen(filename, "w");
        for(int size = 2; size <= MAX_NUM_VERTICES; size *= 2) {
            printf("Testing graph size = %d\n", size);
            float dfsRuntime = 0, kahnRuntime = 0;
            for(int numRun = 0; numRun < NUM_RUN_AVERAGE; numRun++) {
                Graph graph = genGraph(size, density);
                dfsRuntime += dfsTest(&graph);
                // Kahn removes edges so place it last
                kahnRuntime += kahnTest(&graph);
                freeGraph(&graph);
            }
            dfsRuntime /= NUM_RUN_AVERAGE;
            kahnRuntime /= NUM_RUN_AVERAGE;
            fprintf(fp, "%f %f\n", dfsRuntime, kahnRuntime);
        }
        printf("Results written to %s\n", filename);
        fclose(fp);
    }

    for(int depth = 100; depth <= 10000; depth *= 10) {
        printf("Testing graph depth = %d\n", depth);
        sprintf(filename, "stats_depth_%d.txt", depth);
        FILE *fp = fopen(filename, "w");
        for(int size = 2; size <= MAX_NUM_VERTICES; size *= 2) {
            printf("Testing graph size = %d\n", size);
            float dfsRuntime = 0, kahnRuntime = 0;
            for(int numRun = 0; numRun < NUM_RUN_AVERAGE; numRun++) {
                Graph graph = genDepthGraph(size, depth);
                dfsRuntime += dfsTest(&graph);
                // Kahn removes edges so place it last
                kahnRuntime += kahnTest(&graph);
                freeGraph(&graph);
            }
            dfsRuntime /= NUM_RUN_AVERAGE;
            kahnRuntime /= NUM_RUN_AVERAGE;
            fprintf(fp, "%f %f\n", dfsRuntime, kahnRuntime);
        }
        printf("Results written to %s\n", filename);
        fclose(fp);
    }

    return EXIT_SUCCESS;
}
