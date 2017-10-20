#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node {
    char data;
    int neighborsCount;
    struct Node** neighbors;
    bool visited;
} Node;


Node* buildGraph(Node nodes[]) {
    char s[100];
    Node* start = 0;
    while(scanf("%s", s) != EOF) {
        int i = s[0] - 'A';
        if (!start)
            start = &nodes[i];
        nodes[i].neighborsCount = strlen(s) - 2;
        nodes[i].neighbors = malloc(sizeof(Node*) * nodes[i].neighborsCount);
        for (int n = 0; n < nodes[i].neighborsCount; n++) {
            nodes[i].neighbors[n] = &nodes[s[n + 2] - 'A'];
            //printf("%c", nodes[i].neighbors[n]->data);
        }
    }
    return start;
}

typedef struct ListElement {
    Node* data;
    struct ListElement* next;
} ListElement;


ListElement* createListElement(Node* node) {
    ListElement* elem = malloc(sizeof(ListElement));
    elem->data = node;
    elem->next = 0;
    return elem;
}


void bfs(Node* node) {
    ListElement* queue = createListElement(node);
    ListElement* last = queue;
       
    queue->data->visited = true;  

    while (queue) {         
        printf("%c", queue->data->data); 

        for (int n = 0; n < queue->data->neighborsCount; n++) {    
            if (!queue->data->neighbors[n]->visited) {                
                queue->data->neighbors[n]->visited = true;  
                last->next = createListElement(queue->data->neighbors[n]);
                last = last->next;
            }
        }

        queue = queue->next;
    }
}

int main() {    
    Node nodes[26];
    for (int i = 0; i < 26; i++) {
        nodes[i].data = i + 'A';
        nodes[i].neighborsCount = 0;
        nodes[i].visited = false;
    }
 
    Node* start = buildGraph(nodes);
    bfs(start);
    printf("\n");

    return 0;
}