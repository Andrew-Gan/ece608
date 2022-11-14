#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <stdbool.h>

typedef struct {
    bool finished;
    int id;
    int adj[1024];
} Vertex;

typedef struct {
    Vertex *vertices;
    int size;
} Graph;

Graph genGraph(int size);

bool edgeExist(Vertex fromVertex, Vertex toVertex);

bool verifyTopologicalSort(Graph sortedGraph);

void addVertex(Graph *graph, Vertex newVertex);

#endif
