#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define LEARNING_RATE 0.01f
#define MAX_ITERATIONS 10000
#define INIT_FAC 0.01f
#define LOSS_GOAL 0.0001
#define DEBUG true

typedef struct Data {
    float x;
    float output;
} Data;


typedef struct Model {
    float w1;
    float bias;
} Model;

int readTrainData(Data* data) {
    float x;
    float output;
    int i = 0;
    while (scanf("%f,%f", &x, &output) != EOF) {
        if (x == 0 && output == 0)
            break;

        data[i].x = x;
        data[i].output = output;
        i++;
    }
    return i;
}

int readEvalData(Data* data) {
    float x;
    int i = 0;
    while (scanf("%f", &x) != EOF) {
        data[i].x = x;
        i++;
    }
    return i;
}

void normalize(Data* data, int dataSize, float* maxX) {
    if (*maxX == 0) {
        for (int i = 0; i < dataSize; i++) {
            *maxX = max(*maxX, data[i].x);
        }
        if (DEBUG)
            printf("max: %f\n", *maxX);
    }

    for (int i = 0; i < dataSize; i++) {
        data[i].x /= *maxX;
    }
}

void initialize(Model* model) {
    model->w1 = rand() / RAND_MAX * INIT_FAC - INIT_FAC / 2;
    model->bias = rand() / RAND_MAX * INIT_FAC - INIT_FAC / 2;
}

float net(Model* model, float x) {
    return model->w1 * x - model->bias;
}

float forward(Model* model, float x) {
    return net(model, x);
}

float derivative(Model* model, float x) {
    return 1;
}

bool rate(Model* model, Data* data, int dataSize) {
    float loss = 0;
    for (int i = 0; i < dataSize; i++) {
        float output = forward(model, data[i].x);
        loss += pow(output - data[i].output, 2.0);
    }
    if (DEBUG)
        printf("%f, %f - %f\n", model->w1, model->bias, loss);
    return loss <= LOSS_GOAL;
}

void update(Model* model, Data* data, float error) {
    float deriv = derivative(model, data->x);
    model->w1 -= LEARNING_RATE * error * data->x * deriv;
    model->bias -= LEARNING_RATE * error * deriv * -1;
}

void train(Model* model, Data* data, int dataSize) {
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        update(model, &data[i % dataSize], forward(model, data[i % dataSize].x) - data[i % dataSize].output);
        if (rate(model, data, dataSize))
            break;
    }
}

void eval(Model* model, Data* data, int dataSize) {
    for (int i = 0; i < dataSize; i++) {        
        printf("%f\n", forward(model, data[i].x));
    }
}

int main() {
    Data data[1000];

    int n = readTrainData(data);
    float maxX = 0;
    normalize(data, n, &maxX);

    Model model;
    initialize(&model);
    train(&model, data, n);

    n = readEvalData(data);
    normalize(data, n, &maxX);
    eval(&model, data, n);

    return 0;
}