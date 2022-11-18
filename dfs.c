#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "dfs.h"

void _dfsRecurse(Graph *graph, int offset, Graph *sortedGraph) {
    if(graph->vertices[offset].finished == true) {
        return;
    }
    for(int v = 0; v < graph->size; v++) {
        if(edgeExist(&graph->vertices[offset], &graph->vertices[v])) {
            _dfsRecurse(graph, v, sortedGraph);
        }
    }
    graph->vertices[offset].finished = true;
    addVertex(sortedGraph, &graph->vertices[offset], true);
}

float dfsTest(Graph *graph) {
    Graph sortedGraph = {
        .size = 0,
        .capacity = graph->capacity,
        .vertices = malloc(graph->size * sizeof(Vertex))
    };
    time_t start = clock();
    for(int v = graph->size - 1; v >= 0; v--) {
        _dfsRecurse(graph, v, &sortedGraph);
    }
    time_t end = clock();
    assert(verifyTopoSort(&sortedGraph));
    freeGraph(&sortedGraph);
    return (float)(end - start) / CLOCKS_PER_SEC;
}
