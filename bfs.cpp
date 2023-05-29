#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>

#define MAX_NODES 100

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct Graph {
    int numNodes;
    Node* adjacencyList[MAX_NODES];
} Graph;

Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void addEdge(Graph* graph, int src, int dest) {
    Node* newNode = createNode(dest);
    newNode->next = graph->adjacencyList[src];
    graph->adjacencyList[src] = newNode;
}

void BFS(Graph* graph, int startNode) {
    bool visited[MAX_NODES] = { false };

    visited[startNode] = true;
    printf("BFS traversal: %d ", startNode);

    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            Node* queue = createNode(startNode);
            
            while (queue != NULL) {
                #pragma omp task
                {
                    int currentNode = queue->data;
                    Node* neighbor = graph->adjacencyList[currentNode];
                    
                    while (neighbor != NULL) {
                        int nextNode = neighbor->data;
                        if (!visited[nextNode]) {
                            #pragma omp critical
                            {
                                visited[nextNode] = true;
                                printf("%d ", nextNode);
                            }
                            #pragma omp task
                            {
                                Node* newNode = createNode(nextNode);
                                newNode->next = queue->next;
                                queue->next = newNode;
                            }
                        }
                        neighbor = neighbor->next;
                    }
                }
                Node* temp = queue;
                queue = queue->next;
                free(temp);
            }
        }
    }
}

int main() {
    Graph graph;
    graph.numNodes = 7;
    
    for (int i = 0; i < MAX_NODES; i++) {
        graph.adjacencyList[i] = NULL;
    }
    
    addEdge(&graph, 0, 1);
    addEdge(&graph, 0, 2);
    addEdge(&graph, 1, 3);
    addEdge(&graph, 1, 4);
    addEdge(&graph, 2, 5);
    addEdge(&graph, 2, 6);

    printf("Parallel BFS: \n");
    BFS(&graph, 0);

    return 0;
}
