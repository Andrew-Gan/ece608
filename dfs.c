#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "dfs.h"

void _dfsRecurse(Graph graph, int offset, Graph *sortedGraph) {
    for(int v = 0; v < graph.size; v++) {
        if(edgeExist(graph.vertices[offset], graph.vertices[v])) {
            _dfsRecurse(graph, v, sortedGraph);
        }
    }
    graph.vertices[offset].finished = true;
    addVertex(sortedGraph, graph.vertices[offset]);
}

double dfsTest(Graph graph) {
    Graph sortedGraph = {
        .size = 0,
        .vertices = malloc(graph.size * sizeof(Vertex))
    };
    time_t start = clock();
    for(int v = 0; v < graph.size; v++) {
        if(!graph.vertices[v].finished) {
            _dfsRecurse(graph, 0, &sortedGraph);
        }
    }
    time_t end = clock();
    // assert(verifyTopologicalSort(sortedGraph));
    free(sortedGraph.vertices);
    return (double)(end - start) / CLOCKS_PER_SEC;
}
