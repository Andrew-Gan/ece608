#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <stdbool.h>

#define ADJ_LIST_LEN 1024

typedef struct {
    bool finished;
    int id;
    unsigned int adj[ADJ_LIST_LEN];
    int numIncomingEdges;
} Vertex;

typedef struct {
    Vertex *vertices;
    int size;
    int capacity;
} Graph;

typedef enum { DENSITY_25, DENSITY_50, DENSITY_75, DENSITY_100 } MaxDensity;

Graph genGraph(int size, MaxDensity density);

void translateIDToPosition(int id, int *block, int *offset);

bool edgeExist(Vertex *fromVertex, Vertex *toVertex);

void removeEdge(Vertex *fromVertex, Vertex *toVertex);

void addVertex(Graph *graph, Vertex *newVertex, bool insertBack);

void freeGraph(Graph *graph);

bool verifyTopoSort(Graph *sortedGraph);

#endif
