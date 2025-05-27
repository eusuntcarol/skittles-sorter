#define NUM_SAMPLES 51
#define NUM_CLASSES 5
#define NUM_FEATURES 3

#define COLOR_GREEN 0
#define COLOR_ORANGE 1
#define COLOR_PURPLE 2
#define COLOR_RED 3
#define COLOR_YELLOW 4

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

int predict(int* sample) {
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
