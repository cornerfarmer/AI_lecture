#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#define DEBUG true
#define LEARNING_RATE 0.002f
#define MAX_ITERATIONS 10000

typedef struct Point {
    float x;
    float y;
} Point;

int readPoint(Point* data, int* numberOfClusters) {
    float x;
    float y;
    int i = 0;
    scanf("%d", numberOfClusters);
    
    while (scanf("%f,%f", &x, &y) != EOF) {
    
        data[i].x = x;
        data[i].y = y;
        i++;
    }
    return i;
}

void initializePrototypes(Point* data, int dataSize, Point* prototypes, int numberOfClusters) {
    for (int i = 0; i < numberOfClusters; i++) {
        prototypes[i].x = data[i].x;
        prototypes[i].y = data[i].y;
    }
}

float distance(Point* a, Point* b) {
    return sqrt((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
}

Point* findNearest(Point* data, Point* prototypes, int numberOfClusters) {
    float nearestDistance = 0;
    Point* nearestPrototype = 0;
    for (int i = 0; i < numberOfClusters; i++) {
        
        if (!nearestPrototype || distance(&prototypes[i], data) < nearestDistance) {
            nearestDistance = distance(&prototypes[i], data);
            nearestPrototype = &prototypes[i];
        }
    }
    return nearestPrototype;
}


void vectorQuantization(Point* data, int dataSize, Point* prototypes, int numberOfClusters) {
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        Point* randData = &data[(int)((float)rand() / RAND_MAX * (dataSize - 1))];
        Point* nearestPrototype = findNearest(randData, prototypes, numberOfClusters);
        nearestPrototype->x += (randData->x - nearestPrototype->x) * LEARNING_RATE;
        nearestPrototype->y += (randData->y - nearestPrototype->y) * LEARNING_RATE;

        if (DEBUG) {
            for (int p= 0; p < numberOfClusters; p++) {
                printf("%f,%f ", prototypes[p].x, prototypes[p].y);            
            }
            printf("\n");
        }
    }
}

int main() {
    Point data[1000];

    int numberOfClusters;
    int n = readPoint(data, &numberOfClusters);

    Point* prototypes = malloc(sizeof(Point) * numberOfClusters);
    initializePrototypes(data, n, prototypes, numberOfClusters);
    vectorQuantization(data, n, prototypes, numberOfClusters);
    for (int i = 0; i < numberOfClusters; i++) {
        printf("%f,%f\n", prototypes[i].x, prototypes[i].y);
    }

    return 0;
}