#include <stdlib.h>
#include <stdio.h>
#include "dfs.h"

#define MAX_NUM_VERTICES 320
#define NUM_RUN_AVERAGE 3

int main() {
    FILE *fp = fopen("stats.txt", "w");

    for(int size = 100; size <= MAX_NUM_VERTICES; size *= 10) {
        double dfsRuntime = 0;
        // double kahnRuntime = 0;
        for(int numRun = 0; numRun < NUM_RUN_AVERAGE; numRun++) {
            Graph graph = genGraph(size);
            dfsRuntime += dfsTest(graph);
            free(graph.vertices);
            // kahnRuntime += kahnTest(graph);
        }
        dfsRuntime /= NUM_RUN_AVERAGE;
        // kahnRuntime /= NUM_RUN_AVERAGE;
        fprintf(fp, "%f\n", dfsRuntime);
    }

    fclose(fp);

    return EXIT_SUCCESS;
}
