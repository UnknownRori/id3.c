# rid3 - Lightweight ID3 Decision Tree in C

rid3 is a high-performance, memory-optimized implementation of the ID3 (Iterative Dichotomiser 3) 
Machine Learning algorithm written in pure C, with zero dependency except from C Standard Library.

It is designed to be completely domain-agnostic, allowing you to build predictive models for anything from coffee shop recommendations to 
IoT sensor classifications,  all while maintaining strict memory safety and cache-friendly operations.

## 📖 Quick Start

```c
#include <stdio.h>
#include <stdlib.h>
#include "id3.h"

int main() {
    RORI_ID3_ATTRIBUTE_TYPE row1[] = {0, 1, 0}; // Cold, Normal Crowd, Cloudy
    RORI_ID3_ATTRIBUTE_TYPE row2[] = {2, 1, 1}; // Hot, Normal Crowd, Sunny
    RORI_ID3_ATTRIBUTE_TYPE row3[] = {0, 0, 0}; // Cold, Empty, Cloudy

    rid3_Dataset training_data[] = {
        { .label = 1, .values = row1, .len = 3 }, // 1 = Hot Latte
        { .label = 2, .values = row2, .len = 3 }, // 2 = Iced Americano
        { .label = 1, .values = row3, .len = 3 }
    };
    
    size_t total_datasets = sizeof(training_data) / sizeof(training_data[0]);

    printf("Training model...\n");
    rid3_Node* root = rid3_build_decision_tree(training_data, total_datasets);

    // make a prediction on new data
    RORI_ID3_ATTRIBUTE_TYPE current_conditions[] = {2, 1, 1}; // Hot, Normal Crowd, Sunny
    rid3_Dataset prompt = { .label = -1, .values = current_conditions, .len = 3 };

    RORI_ID3_LABEL_TYPE recommendation = rid3_traverse_tree(root, prompt);
    printf("Predicted Label: %d\n", recommendation); // Outputs: 2

    rid3_free_tree(root);

    return 0;
}
```

## 📚 API Reference

I'm sure you can read `id3.h` right?

## 📝 License

Distributed under the MIT License. See LICENSE for more information.
