#include <stdlib.h>
#include "graph.h"

#include <stdio.h>

Graph genGraph(int size) {
    Vertex* vertices = malloc(size * sizeof(Vertex));
    for(int v = 0; v < size; v++) {
        vertices[v].id = v;
        vertices[v].finished = false;
        for(int i = 0; i < 1024; i++) {
            vertices[v].adj[i] = rand();
        }
        // remove all edges to later vertices to prevent cycles
        vertices[v].adj &= ((1 << v) - 1);
        // remove all edges to self to prevent cycles
        vertices[v].adj &= ~(1 << v);
    }
    Graph newGraph = { .vertices = vertices, .size = size };
    return newGraph;
}

bool edgeExist(Vertex fromVertex, Vertex toVertex) {
    return (fromVertex.adj >> toVertex.id) % 2;
}

bool verifyTopologicalSort(Graph sortedGraph) {
    for(int v = 0; v < sortedGraph.size; v++) {
        for(int u = 0; u < v; u++) {
            if(edgeExist(sortedGraph.vertices[v], sortedGraph.vertices[u])) {
                return false;
            }
        }
    }
    return true;
}

void addVertex(Graph *graph, Vertex newVertex) {
    graph->vertices[graph->size] = newVertex;
    graph->size++;
}
