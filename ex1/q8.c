#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

struct Node;

typedef struct Edge {
    int cost;
    struct Node* next;
} Edge;

typedef struct Node {
    char data;
    int edgeCount;
    struct Edge* edges;
    bool visited;
    char* result;
} Node;


Node* buildGraph(Node nodes[]) {
    char s[100];
    Node* start = 0;
    while(scanf("%s", s) != EOF) {
        int i = s[0] - 'A';
        if (!start)
            start = &nodes[i];
        nodes[i].edgeCount++;
        Edge* newEdges = malloc(sizeof(Edge) * nodes[i].edgeCount);

        if (nodes[i].edges) {
            for (int n = 0; n < nodes[i].edgeCount - 1; n++) {
                newEdges[n] = nodes[i].edges[n];
            }
            free(nodes[i].edges);
        }
        
        nodes[i].edges = newEdges;
        nodes[i].edges[nodes[i].edgeCount - 1].next = &nodes[s[2] - 'A'];
        nodes[i].edges[nodes[i].edgeCount - 1].cost = atoi(s + 4);
      //  printf("%c", nodes[i].edges[nodes[i].edgeCount - 1].next->data);
    }
    return start;
}

typedef struct ListElement {
    Node* data;
    struct ListElement* next;
    int cost;
    struct ListElement* prev;
} ListElement;


ListElement* createListElement(Node* node, ListElement* next, int cost, ListElement* prev) {
    ListElement* elem = malloc(sizeof(ListElement));
    elem->data = node;
    elem->next = next;
    elem->cost = cost;
    elem->prev = prev;
    return elem;
}

ListElement* insertSorted(ListElement* start, ListElement* elem) {
    if (!start)
        return elem;

    if (start->cost > elem->cost) {
        elem->next = start;
        return elem;
    } else {
        start->next = insertSorted(start->next, elem);
        return start;
    }
}


void dijkstra(Node* node) {
    ListElement* queue = createListElement(node, 0, 0, 0);
    
    while (queue) {         
        if (!queue->data->visited) {
            queue->data->visited = true;
            ListElement* print = queue;
            int i = 0;
            while (print) {
                i += sprintf(queue->data->result + i, "%c-", print->data->data);    
                print = print->prev;
            }
            sprintf(queue->data->result + i, "%d\n", queue->cost);

            for (int n = queue->data->edgeCount - 1; n >= 0; n--) {    
                if (!queue->data->edges[n].next->visited) {       
                    queue = insertSorted(queue, createListElement(queue->data->edges[n].next, 0, queue->cost + queue->data->edges[n].cost, queue));
                }
            }
        }       

        queue = queue->next;
    }
}

int main() {    
    Node nodes[26];
    for (int i = 0; i < 26; i++) {
        nodes[i].data = i + 'A';
        nodes[i].edgeCount = 0;
        nodes[i].visited = false;
        nodes[i].edges = 0;
        nodes[i].result = calloc(100, sizeof(char));
    }
 
    Node* start = buildGraph(nodes);
    dijkstra(start);

    for (int i = 0; i < 26; i++) {
        if (nodes[i].visited)
            printf("%s", nodes[i].result);
    }

    return 0;
}