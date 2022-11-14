#include <stdlib.h>
#include "graph.h"

#include <stdio.h>

Graph genGraph(int size) {
    Vertex* vertices = malloc(size * sizeof(Vertex));
    for(int v = 0; v < size; v++) {
        vertices[v].id = v;
        vertices[v].finished = false;
        for(int i = 0; i < 4; i++) {
            vertices[v].adj[i] = rand();
        }
        // remove all edges to later vertices to prevent cycles
        int adjIndex = v / (8 * sizeof(int));
        int adjOffset = v % (8 * sizeof(int));
        vertices[v].adj[adjIndex] &= ((1 << adjOffset) - 1);
        // remove all edges to self to prevent cycles
        vertices[v].adj[adjIndex] &= ~(1 << adjOffset);
    }
    Graph newGraph = { .vertices = vertices, .size = size };
    return newGraph;
}

bool edgeExist(Vertex fromVertex, Vertex toVertex) {
    int adjIndex = toVertex.id / (8 * sizeof(int));
    int adjOffset = toVertex.id % (8 * sizeof(int));
    return (fromVertex.adj[adjIndex] >> adjOffset) % 2;
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
