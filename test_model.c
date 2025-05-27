#include <stdio.h>
#include <math.h>

#define NUM_CLASSES 5
#define NUM_FEATURES 3
#define NUM_SAMPLES 15

const char* class_names[NUM_CLASSES] = {"GREEN", "ORANGE", "PURPLE", "RED", "YELLOW"};

const float weights[NUM_CLASSES][NUM_FEATURES] = {
  {0.591368, -0.640231, -0.069706},  // GREEN
  {-0.659359, 0.359500, 0.204089},   // ORANGE
  {0.454038, -0.008298, -0.460773},  // PURPLE
  {-0.191400, 0.445365, 0.014858},   // RED
  {-0.194646, -0.156336, 0.311533},  // YELLOW
};

const float bias[NUM_CLASSES] = {49.027954, -6.498018, 18.068558, -68.343690, 7.745196};

float custom_exp(float x) {
    if (x > 10.0f) x = 10.0f;
    if (x < -10.0f) x = -10.0f;

    float term = 1.0f;
    float result = 1.0f;
    float x_power = x;
    // Taylor series: 1 + x + x^2/2 + x^3/6
    result += x;
    x_power *= x;
    result += x_power / 2.0f;
    x_power *= x;
    result += x_power / 6.0f;
    return result;
}

void softmax(float* logits, float* probs, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        probs[i] = custom_exp(logits[i]);
        sum += probs[i];
    }
    for (int i = 0; i < n; i++) {
        probs[i] /= sum;
    }
}

int predict(float* sample) {
    float logits[NUM_CLASSES];
    float probs[NUM_CLASSES];

    for (int c = 0; c < NUM_CLASSES; c++) {
        logits[c] = bias[c];
        for (int f = 0; f < NUM_FEATURES; f++) {
            logits[c] += weights[c][f] * sample[f];
        }
    }

    softmax(logits, probs, NUM_CLASSES);

    int max_class = 0;
    float max_prob = probs[0];
    for (int c = 1; c < NUM_CLASSES; c++) {
        if (probs[c] > max_prob) {
            max_prob = probs[c];
            max_class = c;
        }
    }

    return max_class;
}

int main() {
    float samples[NUM_SAMPLES][NUM_FEATURES] = {
        {241, 289, 248},
        {247, 290, 241},
        {246, 283, 247},
        {242, 289, 248},
        {246, 290, 244},
        {247, 289, 248},
        {242, 289, 248},
        {247, 290, 247},
        {248, 289, 247},
        {187, 231, 227},
        {193, 231, 227},
        {193, 231, 221},
        {193, 231, 227},
        {193, 231, 227},
        {193, 231, 227},
    };

    for (int i = 0; i < NUM_SAMPLES; i++) {
        int predicted_class = predict(samples[i]);
        printf("Sample %d: Predicted class = %s\n", i + 1, class_names[predicted_class]);
    }

    return 0;
}
