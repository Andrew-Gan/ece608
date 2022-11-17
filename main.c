#include <stdlib.h>
#include <stdio.h>
#include "dfs.h"
#include "kahn.h"

#define MAX_NUM_VERTICES 32768
#define NUM_RUN_AVERAGE 3

int main() {
    for(MaxDensity d = DENSITY_25; d <= DENSITY_100; d++) {
        printf("Testing graph density option = %d\n", d);
        char filename[32];
        sprintf(filename, "stats_%d.txt", d);
        FILE *fp = fopen(filename, "w");
        for(int size = 2; size <= MAX_NUM_VERTICES; size *= 2) {
            printf("Testing graph size = %d\n", size);
            float dfsRuntime = 0, kahnRuntime = 0;
            for(int numRun = 0; numRun < NUM_RUN_AVERAGE; numRun++) {
                Graph graph = genGraph(size, d);
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
