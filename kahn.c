#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "kahn.h"

void _getRootVertices(Graph *graph, unsigned int *rootVertexSet) {
    for(int v = 0; v < graph->size; v++) {
        Vertex *currVertex = &graph->vertices[v];
        if(currVertex->numIncomingEdges == 0) {
            int block = 0, offset = 0;
            translateIDToPosition(currVertex->id, &block, &offset);
            rootVertexSet[block] |= 1 << offset;
        }
    }
}

int _popRootVertex(Graph *graph, unsigned int *rootVertexSet) {
    for(int v = 0; v < graph->size; v++) {
        Vertex *currVertex = &graph->vertices[v];
        int block = 0, offset = 0;
        translateIDToPosition(currVertex->id, &block, &offset);
        if(rootVertexSet[block] & ( 1 << offset)) {
            rootVertexSet[block] &= ~(1 << offset);
            return v;
        }
    }
    return -1;
}

float kahnTest(Graph *graph) {
    Graph sortedGraph = {
        .size = 0,
        .capacity = graph->capacity,
        .vertices = malloc(graph->size * sizeof(Vertex))
    };
    unsigned int rootVertexSet[ADJ_LIST_LEN];
    for(int i = 0; i < ADJ_LIST_LEN; i++) {
        rootVertexSet[i] = 0;
    }

    time_t start = clock();
    _getRootVertices(graph, rootVertexSet);
    int rootVertexID = _popRootVertex(graph, rootVertexSet);
    while(rootVertexID != -1) {
        Vertex *rootVertex = &graph->vertices[rootVertexID];
        addVertex(&sortedGraph, rootVertex, false);
        for(int v = 0; v < graph->size; v++) {
            if(edgeExist(rootVertex, &graph->vertices[v])) {
                Vertex *adjVertex = &graph->vertices[v];
                removeEdge(rootVertex, adjVertex);
                if(adjVertex->numIncomingEdges == 0) {
                    addVertex(&sortedGraph, adjVertex, false);
                }
            }
        }
        rootVertexID = _popRootVertex(graph, rootVertexSet);
    }
    time_t end = clock();
    assert(verifyTopoSort(&sortedGraph));
    freeGraph(&sortedGraph);
    return (float)(end - start) / CLOCKS_PER_SEC;
}
