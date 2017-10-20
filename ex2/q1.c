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
#define ACCURACY_GOAL 0.999
#define DEBUG false

typedef struct Data {
    float x;
    float y;
    int output;
} Data;


typedef struct Model {
    float w1;
    float w2;
    float bias;
} Model;

int readTrainData(Data* data) {
    float x;
    float y;
    int output;
    int i = 0;
    while (scanf("%f,%f,%d", &x, &y, &output) != EOF) {
        if (output == 0)
            break;

        data[i].x = x;
        data[i].y = y;
        data[i].output = output;
        i++;
    }
    return i;
}

int readEvalData(Data* data) {
    float x;
    float y;
    int i = 0;
    while (scanf("%f,%f", &x, &y) != EOF) {
        data[i].x = x;
        data[i].y = y;
        i++;
    }
    return i;
}

void normalize(Data* data, int dataSize, float* maxX, float* maxY) {
    if (*maxX == 0 || *maxY == 0) {
        for (int i = 0; i < dataSize; i++) {
            *maxX = max(*maxX, data[i].x);
            *maxY = max(*maxY, data[i].y);
        }
        if (DEBUG)
            printf("max: %f, %f\n", *maxX, *maxY);
    }

    for (int i = 0; i < dataSize; i++) {
        data[i].x /= *maxX;
        data[i].y /= *maxY;
    }
}

void initialize(Model* model) {
    model->w1 = rand() / RAND_MAX * INIT_FAC - INIT_FAC / 2;
    model->w2 = rand() / RAND_MAX * INIT_FAC - INIT_FAC / 2;
    model->bias = rand() / RAND_MAX * INIT_FAC - INIT_FAC / 2;
}

float net(Model* model, float x, float y) {
    return model->w1 * x + model->w2 * y - model->bias;
}

float forward(Model* model, float x, float y) {
    return tanh(net(model, x, y));
}

float derivative(Model* model, float x, float y) {
    return 1 / pow(cosh(net(model, x, y)), 2.0);
}

bool rate(Model* model, Data* data, int dataSize) {
    float loss = 0;
    int correct = 0;
    for (int i = 0; i < dataSize; i++) {
        float output = forward(model, data[i].x, data[i].y);
        loss += pow(output - data[i].output, 2.0);
        correct += (output < 0 ? -1 : 1) == data[i].output;
    }
    float accuracy = (float)correct / dataSize;
    if (DEBUG)
        printf("%f, %f, %f - %f, %f\n", model->w1, model->w2, model->bias, loss, accuracy);
    return accuracy >= ACCURACY_GOAL;
}

void update(Model* model, Data* data, float error) {
    float deriv = derivative(model, data->x, data->y);
    model->w1 -= LEARNING_RATE * error * data->x * deriv;
    model->w2 -= LEARNING_RATE * error * data->y * deriv;
    model->bias -= LEARNING_RATE * error * deriv * -1;
}

void train(Model* model, Data* data, int dataSize) {
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        update(model, &data[i % dataSize], forward(model, data[i % dataSize].x, data[i % dataSize].y) - data[i % dataSize].output);
        if (rate(model, data, dataSize))
            break;
    }
}

void eval(Model* model, Data* data, int dataSize) {
    for (int i = 0; i < dataSize; i++) {        
        printf("%s\n", forward(model, data[i].x, data[i].y) < 0 ? "-1" : "+1");
    }
}

int main() {
    Data data[1000];

    int n = readTrainData(data);
    float maxX = 0;
    float maxY = 0;
    normalize(data, n, &maxX, &maxY);

    Model model;
    initialize(&model);
    train(&model, data, n);

    n = readEvalData(data);
    normalize(data, n, &maxX, &maxY);
    eval(&model, data, n);

    return 0;
}