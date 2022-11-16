#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "kahn.h"

void _getRootVertices(Graph *graph, unsigned int *rootVertexSet) {
    for(int v = 0; v < graph->size; v++) {
        Vertex *currVertex = &graph->vertices[v];
        if(currVertex->isRoot) {
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

double kahnTest(Graph *graph) {
    Graph sortedGraph = {
        .size = 0,
        .vertices = malloc(graph->size * sizeof(Vertex))
    };
    unsigned int rootVertexSet[ADJ_LIST_LEN];
    for(int i = 0; i < ADJ_LIST_LEN; i++) {
        rootVertexSet[i] = 0;
    }
    _getRootVertices(graph, rootVertexSet);

    time_t start = clock();
    int rootVertexID = _popRootVertex(graph, rootVertexSet);
    while(rootVertexID != -1) {
        Vertex *rootVertex = &graph->vertices[rootVertexID];
        addVertex(&sortedGraph, rootVertex);
        for(int v = 0; v < graph->size; v++) {
            if(edgeExist(rootVertex, &graph->vertices[v])) {
                Vertex *adjVertex = &graph->vertices[v];
                removeEdge(rootVertex, adjVertex);
                if(hasNoIncomingEdge(graph, adjVertex)) {
                    addVertex(&sortedGraph, adjVertex);
                }
            }
        }
        rootVertexID = _popRootVertex(graph, rootVertexSet);
    }
    time_t end = clock();
    // assert(verifyTopoSort(&sortedGraph));
    freeGraph(&sortedGraph);
    return (double)(end - start) / CLOCKS_PER_SEC;
}
