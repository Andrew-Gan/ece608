#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include "graph.h"

#define NUM_THREAD 16

unsigned int densityToNumBits[4] = {
    0b10001000100010001000100010001000,
    0b10101010101010101010101010101010,
    0b11101110111011101110111011101110,
    0b11111111111111111111111111111111,
};

void translateIDToPosition(int id, int *block, int *offset) {
    *block = id / (8 * sizeof(int));
    *offset = id % (8 * sizeof(int));
}

unsigned int _randuint() {
    return ((unsigned int)rand() << 1) | (rand() & 1);
}

typedef struct {
    int index;
    int workload;
    MaxDensity density;
    Graph *graph;
} ParallelConnectorArg;

void* _parallel_connector(void *args) {
    ParallelConnectorArg myargs = *(ParallelConnectorArg*)args;
    int start = myargs.index * myargs.workload;
    for(int v = start; v < start + myargs.workload; v++) {
        Vertex *newVertex = &myargs.graph->vertices[v];
        newVertex->id = v;
        newVertex->finished = false;
        for(int i = 0; i < ADJ_LIST_LEN; i++) {
            newVertex->adj[i] = _randuint() & densityToNumBits[myargs.density];
        }
        // remove all edges to later vertices to prevent cycles
        int adjIndex = 0, adjOffset = 0;
        translateIDToPosition(v, &adjIndex, &adjOffset);
        newVertex->adj[adjIndex] &= ((1 << adjOffset) - 1);
        for(int i = adjIndex + 1; i < ADJ_LIST_LEN; i++) {
            newVertex->adj[i] &= 0;
        }
        // remove all edges to self to prevent cycles
        newVertex->adj[adjIndex] &= ~(1 << adjOffset);
    }
    return NULL;
}

typedef struct {
    int index;
    int workload;
    Graph *graph;
} RootFinderArg;

void* _parallel_root_finder(void *args) {
    RootFinderArg myargs = *(RootFinderArg*) args;
    int start = myargs.index * myargs.workload;
    for(int v = start; v < start + myargs.workload; v++) {
        Vertex *vertex = &myargs.graph->vertices[v];
        vertex->isRoot = true;
        for(int u = 0; u < myargs.graph->size && vertex->isRoot == true; u++) {
            if(edgeExist(&myargs.graph->vertices[u], vertex)) {
                vertex->isRoot = false;
            }
        }
    }
    return NULL;
}

Graph genGraph(int size, MaxDensity density) {
    Graph newGraph = { .size = size, .vertices = malloc(size * sizeof(Vertex)) };
    int numThread = size / NUM_THREAD < 1 ? size : NUM_THREAD;
    int workload = size / numThread ;

    // randomly connect vertices
    pthread_t *threads = malloc(numThread * sizeof(pthread_t));
    ParallelConnectorArg *args = malloc(numThread * sizeof(ParallelConnectorArg));
    for(int t = 0; t < numThread; t++) {
        ParallelConnectorArg newArg = { .density = density, .graph = &newGraph, .index = t, .workload = workload };
        args[t] = newArg;
        pthread_create(&threads[t], NULL, _parallel_connector, &args[t]);
    }
    for(int t = 0; t < numThread; t++) {
        pthread_join(threads[t], NULL);
    }
    free(threads);
    free(args);

    // find root vertices
    pthread_t *threads2 = malloc(numThread * sizeof(pthread_t));
    RootFinderArg *args2 = malloc(numThread * sizeof(RootFinderArg));
    for(int t = 0; t < numThread; t++) {
        RootFinderArg newArg = { .index = t, .workload = workload, .graph = &newGraph };
        args2[t] = newArg;
        pthread_create(&threads2[t], NULL, _parallel_root_finder, &args2[t]);
    }
    for(int t = 0; t < numThread; t++) {
        pthread_join(threads2[t], NULL);
    }
    free(threads2);
    free(args2);
    return newGraph;
}

bool edgeExist(Vertex *fromVertex, Vertex *toVertex) {
    int adjIndex = 0, adjOffset = 0;
    translateIDToPosition(toVertex->id, &adjIndex, &adjOffset);
    return (fromVertex->adj[adjIndex] >> adjOffset) & 1;
}

bool hasNoIncomingEdge(Graph *graph, Vertex *toVertex) {
    for(int v = 0; v < graph->size; v++) {
        if(edgeExist(&graph->vertices[v], toVertex)) {
            return false;
        }
    }
    return true;
}

void removeEdge(Vertex *fromVertex, Vertex *toVertex) {
    int adjIndex = 0, adjOffset = 0;
    translateIDToPosition(toVertex->id, &adjIndex, &adjOffset);
    fromVertex->adj[adjIndex] &= ~(1 << adjOffset);
}

void addVertex(Graph *graph, Vertex *newVertex) {
    graph->vertices[graph->size].id = newVertex->id;
    graph->vertices[graph->size].finished = newVertex->finished;
    for(int i = 0; i < ADJ_LIST_LEN; i++) {
        graph->vertices[graph->size].adj[i] = newVertex->adj[i];
    }
    graph->size++;
}

void freeGraph(Graph *graph) {
    free(graph->vertices);
}

bool verifyTopoSort(Graph *sortedGraph) {
    for(int v = 0; v < sortedGraph->size; v++) {
        for(int u = 0; u < v; u++) {
            if(edgeExist(&sortedGraph->vertices[v], &sortedGraph->vertices[u])) {
                return false;
            }
        }
    }
    return true;
}
