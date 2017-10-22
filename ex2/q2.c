#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define LEARNING_RATE 0.3f
#define MOMENTUM 0.0f
#define MAX_ITERATIONS 20000
#define INIT_FAC 0.001f
#define ACCURACY_GOAL 0.99
#define DEBUG true
#define NUMBER_OF_LAYERS 3
#define LAYERS {2, 16, 1}
#define BATCH_SIZE 10
#define GOAL_CORRECT_ITERATIONS 50

typedef struct Data {
    float* input;
    int* output;
} Data;

typedef struct Layer {
    int numberOfNeurons;
    float** weights;
    float* biases;
    float** gradWeights;
    float* gradBiases;
    float* net;
    float* y;
    float* delta;
} Layer;

typedef struct Model {
    Layer* layers;
    int numberOfLayers;
    int inputSize;
} Model;

int readTrainData(Data* data, int inputSize, int outputSize) {
    float x;
    float y;
    int output;
    int i = 0;

    while (scanf("%f,%f,%d", &x, &y, &output) != EOF) {
        if (output == 0)
            break;

        data[i].input = malloc(sizeof(float) * inputSize);
        data[i].input[0] = x;
        data[i].input[1] = y;
        data[i].output = malloc(sizeof(int) * outputSize);
        data[i].output[0] = output;
        i++;
    }
    return i;
}

int readEvalData(Data* data, int inputSize) {
    float x;
    float y;
    int i = 0;
    while (scanf("%f,%f", &x, &y) != EOF) {
        
        data[i].input = malloc(sizeof(float) * inputSize);
        data[i].input[0] = x;
        data[i].input[1] = y;
        i++;
    }
    return i;
}

void normalize(Data* data, int dataSize, float* maxValues, int inputSize) {
    if (maxValues[0] == 0) {
        for (int i = 0; i < dataSize; i++) {
            for (int j = 0; j < inputSize; j++)
                maxValues[j] = max(maxValues[j], data[i].input[j]);
        }
        if (DEBUG)
            printf("max: %f, %f\n", maxValues[0], maxValues[1]);
    }

    for (int i = 0; i < dataSize; i++) {
        for (int j = 0; j < inputSize; j++)
            data[i].input[j] /= maxValues[j];        
    }
}

void initialize(Model* model, int* layers, int numberOfLayers) {
    model->numberOfLayers = numberOfLayers - 1;
    model->inputSize = layers[0];
    model->layers = malloc(sizeof(Layer) * (numberOfLayers - 1));

    int neuronsInPrevLayer = model->inputSize;
    for (int i = 0; i < numberOfLayers - 1; i++) {
        model->layers[i].numberOfNeurons = layers[i + 1];

        model->layers[i].weights = malloc(sizeof(float*) * layers[i + 1]);
        model->layers[i].gradWeights = malloc(sizeof(float*) * layers[i + 1]);
        model->layers[i].biases = malloc(sizeof(float) * layers[i + 1]);
        model->layers[i].gradBiases = malloc(sizeof(float) * layers[i + 1]);
        model->layers[i].net = malloc(sizeof(float) * layers[i + 1]);
        model->layers[i].y = malloc(sizeof(float) * layers[i + 1]);
        model->layers[i].delta = malloc(sizeof(float) * layers[i + 1]);
        for (int n = 0; n < layers[i + 1]; n++) {
            model->layers[i].biases[n] = 0;
            model->layers[i].gradBiases[n] = 0;

            model->layers[i].weights[n] = malloc(sizeof(float) * neuronsInPrevLayer);
            model->layers[i].gradWeights[n] = malloc(sizeof(float) * neuronsInPrevLayer);
            for (int l = 0; l < neuronsInPrevLayer; l++) {
                model->layers[i].weights[n][l] = (float)rand() / RAND_MAX * INIT_FAC - INIT_FAC / 2;
                model->layers[i].gradWeights[n][l] = 0;
            }
        }

        neuronsInPrevLayer = layers[i + 1];
    }
}

float* forward(Model* model, float* input) {
    int neuronsInPrevLayer = model->inputSize;
    float* prevLayerY = input;
    for (int i = 0; i < model->numberOfLayers; i++) {
        for (int n = 0; n < model->layers[i].numberOfNeurons; n++) {
            model->layers[i].net[n] = -model->layers[i].biases[n];
            for (int l = 0; l < neuronsInPrevLayer; l++) {
                model->layers[i].net[n] += model->layers[i].weights[n][l] * prevLayerY[l];
            }
            model->layers[i].y[n] = tanh(model->layers[i].net[n]);
        }

        neuronsInPrevLayer = model->layers[i].numberOfNeurons;
        prevLayerY = model->layers[i].y;
    }
    return prevLayerY;
}

float derivative(float net) {
    return 1 / pow(cosh(net), 2.0);
}

float calcLoss(Model* model, Data* data, int dataSize) {
    float loss = 0;
    for (int i = 0; i < dataSize; i++) {
        float* output = forward(model, data[i].input);
        for (int j = 0; j < model->layers[model->numberOfLayers - 1].numberOfNeurons; j++) {
            loss += 0.5f * pow(output[j] - data[i].output[j], 2.0);
        }
    }
    return loss;
}

bool rate(Model* model, Data* data, int dataSize, int epoch) {
    float loss = 0;
    int correct = 0;
    for (int i = 0; i < dataSize; i++) {
        float* output = forward(model, data[i].input);
        for (int j = 0; j < model->layers[model->numberOfLayers - 1].numberOfNeurons; j++) {
            loss += 0.5f * pow(output[j] - data[i].output[j], 2.0);
            correct += (output[j] < 0 ? -1 : 1) == data[i].output[j];
        }
    }
    loss /= dataSize;
    float accuracy = (float)correct / (model->layers[model->numberOfLayers - 1].numberOfNeurons * dataSize);
    if (DEBUG)
        printf("%d: %f, %f, %d\n", epoch, loss, accuracy, model->layers[model->numberOfLayers - 1].numberOfNeurons);
    return accuracy >= ACCURACY_GOAL;
}

void calcGradients(Model* model, float* error, float* input) {
    int neuronsInPrevLayer;
    float* activationsOfPrevLayer;
    for (int i = model->numberOfLayers - 1; i >= 0; i--) {
        neuronsInPrevLayer = i > 0 ? model->layers[i - 1].numberOfNeurons : model->inputSize;
        activationsOfPrevLayer = i > 0 ? model->layers[i - 1].y : input;

        for (int n = 0; n < model->layers[i].numberOfNeurons; n++) {
            if (i == model->numberOfLayers - 1)
                model->layers[i].delta[n] = error[n] * derivative(model->layers[i].net[n]);
            else {
                model->layers[i].delta[n] = 0;
                for (int o = 0; o < model->layers[i + 1].numberOfNeurons; o++) {
                    model->layers[i].delta[n] += model->layers[i + 1].delta[o] * model->layers[i + 1].weights[o][n];
                }
                model->layers[i].delta[n] *= derivative(model->layers[i].net[n]);
            }
            for (int l = 0; l < neuronsInPrevLayer; l++) { 
                float delta = LEARNING_RATE * model->layers[i].delta[n] * activationsOfPrevLayer[l];
                model->layers[i].gradWeights[n][l] -= (1 - MOMENTUM) * delta;
                //if (i + n + l == 0)
                    //printf("%d %d %d %f\n", i, n, l,model->layers[i].weights[n][l]);
            }
            model->layers[i].gradBiases[n] -= (1 - MOMENTUM) * LEARNING_RATE * model->layers[i].delta[n] * -1;
         //   printf("%d %d %f\n", i, n, model->layers[i].delta[n]);
        }
    }
   // exit(0);
}



void resetGradients(Model* model) {
    int neuronsInPrevLayer;

    for (int i = model->numberOfLayers - 1; i >= 0; i--) {
        neuronsInPrevLayer = i > 0 ? model->layers[i - 1].numberOfNeurons : model->inputSize;

        for (int n = 0; n < model->layers[i].numberOfNeurons; n++) {
            for (int l = 0; l < neuronsInPrevLayer; l++) { 
                model->layers[i].gradWeights[n][l] *= MOMENTUM;
            }
            model->layers[i].gradBiases[n] *= MOMENTUM;
        }
    }
}


void update(Model* model, int dataSize) {
    int neuronsInPrevLayer;
    for (int i = model->numberOfLayers - 1; i >= 0; i--) {
        neuronsInPrevLayer = i > 0 ? model->layers[i - 1].numberOfNeurons : model->inputSize;

        for (int n = 0; n < model->layers[i].numberOfNeurons; n++) {
            for (int l = 0; l < neuronsInPrevLayer; l++) { 
                model->layers[i].weights[n][l] += 1.0f / BATCH_SIZE * model->layers[i].gradWeights[n][l];
            }
            model->layers[i].biases[n] += 1.0f / BATCH_SIZE * model->layers[i].gradBiases[n];
        }
    }
}


void train(Model* model, Data* data, int dataSize) {
    float* error = malloc(sizeof(float) * model->layers[model->numberOfLayers - 1].numberOfNeurons);
    int i = 0;
    int epoch = 0;
    int correctIterations = 0;
    for (int t = 0; t < MAX_ITERATIONS; t++) {
        resetGradients(model);
        int correct = 0;
        float loss = 0;
        for (int b = 0; b < BATCH_SIZE; b++) {
            float* output = forward(model, data[i].input);
            for (int e = 0; e < model->layers[model->numberOfLayers - 1].numberOfNeurons; e++) {
                error[e] = output[e] - data[i].output[e];
                correct += (output[e] < 0 ? -1: 1) == data[i].output[e];
                loss += error[e] * error[e];
            }
            calcGradients(model, error, data[i].input);        
            i++;
            i %= dataSize;
            if (i == 0)
                epoch++;
        }
        update(model, dataSize);
        if (DEBUG)
            printf("%d %f %f\n", epoch, loss / BATCH_SIZE, (float)correct / BATCH_SIZE);
        if (correct == BATCH_SIZE) {
            correctIterations++;
            if (correctIterations > GOAL_CORRECT_ITERATIONS)
                break;
        } else {
            correctIterations = 0;
        }
        //if (rate(model, data, dataSize, epoch))
        //    break;
    }
}

void eval(Model* model, Data* data, int dataSize) {
    for (int i = 0; i < dataSize; i++) {        
        printf("%s\n", forward(model, data[i].input)[0] < 0 ? "-1" : "+1");
    }
}

int main() {
    srand(time(NULL));

    Data data[1000];
    int layers[NUMBER_OF_LAYERS] = LAYERS;

    int dataSize = readTrainData(data, layers[0], layers[NUMBER_OF_LAYERS - 1]);
    float* maxValues = malloc(sizeof(float) * layers[0]);
    normalize(data, dataSize, maxValues, layers[0]);

    Model model;
    initialize(&model, layers, NUMBER_OF_LAYERS);
/*
    float* error = malloc(sizeof(float) * model.layers[model.numberOfLayers - 1].numberOfNeurons);
    

    for (int i = 0; i < dataSize; i++) {
        float* output = forward(&model, data[i].input);
        for (int e = 0; e < 1; e++) 
            error[e] = output[e] - data[i].output[e];
        calcGradients(&model, error, data[i].input);
    }

    int neuronsInPrevLayer;
    float e = 0.1f;
    
    for (int i = model.numberOfLayers - 1; i >= 0; i--) {
        neuronsInPrevLayer = i > 0 ? model.layers[i - 1].numberOfNeurons : model.inputSize;

        for (int n = 0; n < model.layers[i].numberOfNeurons; n++) {
            for (int l = 0; l < neuronsInPrevLayer; l++) { 
                model.layers[i].weights[n][l] += e;
                float prevLoss = calcLoss(&model, data, dataSize);
                model.layers[i].weights[n][l] -= 2 * e;
                float nextLoss = calcLoss(&model, data, dataSize);
                model.layers[i].weights[n][l] += e;
                printf("%d %d %d %f %f\n", i, n, l, model.layers[i].gradWeights[n][l], (nextLoss - prevLoss) / (2*e));
            }

            model.layers[i].biases[n] += e;
            float prevLoss = calcLoss(&model, data, dataSize);
            model.layers[i].biases[n] -= 2 * e;
            float nextLoss = calcLoss(&model, data, dataSize);
            model.layers[i].biases[n] += e;
            printf("%d %d %f %f\n", i, n, model.layers[i].gradBiases[n], (nextLoss - prevLoss) / (2*e));
        }
    }
    return 0;*/
    
    train(&model, data, dataSize);

    if (DEBUG) {
        float* input = malloc(sizeof(float) * 2);
        for (float y = 1; y >= -1; y-=0.08) {
            for (float x = -1; x < 1; x+=0.02) {
                input[0] = x;
                input[1] = y;
                printf("%s", (forward(&model, input)[0] < 0 ? "-" : "+"));
            }

            printf("\n");
        }
    }
    dataSize = readEvalData(data, layers[0]);
    normalize(data, dataSize, maxValues, layers[0]);
    eval(&model, data, dataSize);

    return 0;
}