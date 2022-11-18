#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include "graph.h"

#include <stdio.h>

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
    unsigned int randVal = 0;
    for(int i = 0; i < 3; i++) {
        randVal |= ((unsigned int)rand() << 1) | (rand() & 1);
    }
    return randVal;
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
        vertex->numIncomingEdges = 0;
        for(int u = 0; u < myargs.graph->size; u++) {
            if(edgeExist(&myargs.graph->vertices[u], vertex)) {
                vertex->numIncomingEdges++;
            }
        }
    }
    return NULL;
}

Graph genGraph(int size, MaxDensity density) {
    Graph newGraph = { .size = size, .capacity = size, .vertices = malloc(size * sizeof(Vertex)) };
    int numThread = size < NUM_THREAD ? 1 : NUM_THREAD;
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

Graph genDepthGraph(int size, int depth) {
    Graph newGraph = { .size = size, .capacity = size, .vertices = malloc(size * sizeof(Vertex)) };

    for(int v =  0; v < newGraph.size; v++) {
        newGraph.vertices[v].id = v;
        newGraph.vertices[v].finished = false;
        newGraph.vertices[v].numIncomingEdges = 0;
        for(int i = 0; i < ADJ_LIST_LEN; i++) {
            newGraph.vertices[v].adj[i] = 0;
        }
    }

    for(int v = 0; v < size; v += depth) {
        for(int d = 1; d < depth; d++) {
            int id = v * depth + d;
            if(id >= newGraph.size) {
                break;
            }
            int block = 0, offset = 0;
            translateIDToPosition(id, &block, &offset);
            newGraph.vertices[id - 1].adj[block] |= 1 << offset;
            newGraph.vertices[id - 1].numIncomingEdges++;
        }
    }

    return newGraph;
}

bool edgeExist(Vertex *fromVertex, Vertex *toVertex) {
    int adjIndex = 0, adjOffset = 0;
    translateIDToPosition(toVertex->id, &adjIndex, &adjOffset);
    return (fromVertex->adj[adjIndex] >> adjOffset) & 1;
}

void removeEdge(Vertex *fromVertex, Vertex *toVertex) {
    if(toVertex->numIncomingEdges == 0) {
        return;
    }
    int adjIndex = 0, adjOffset = 0;
    translateIDToPosition(toVertex->id, &adjIndex, &adjOffset);
    fromVertex->adj[adjIndex] &= ~(1 << adjOffset);
    toVertex->numIncomingEdges--;
}

void addVertex(Graph *graph, Vertex *newVertex, bool insertBack) {
    if(insertBack) {
        graph->vertices[graph->capacity - graph->size - 1] = *newVertex;
        for(int i = 0; i < ADJ_LIST_LEN; i++) {
            graph->vertices[graph->capacity - graph->size - 1].adj[i] = newVertex->adj[i];
        }
    }
    else {
        graph->vertices[graph->size] = *newVertex;
        for(int i = 0; i < ADJ_LIST_LEN; i++) {
            graph->vertices[graph->size].adj[i] = newVertex->adj[i];
        }
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
                printf("%d -> %d\n", v, u);
                return false;
            }
        }
    }
    return true;
}
