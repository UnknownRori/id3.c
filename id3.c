#include <assert.h>
#include <math.h>
#include "id3.h"

static float calculate_entropy(rid3_Dataset* datasets, size_t len);
static float calculate_information_gain(rid3_Dataset* datasets, size_t len);
static rid3_Node* __rid3_build_decision_tree(
    rid3_Dataset* datasets,
    size_t        datasets_len,
    RORI_ID3_LABEL_TYPE* attributes,
    size_t attributes_len
);

rid3_Node* rid3_build_decision_tree(
    rid3_Dataset* datasets,
    size_t        datasets_len
) {
    assert(datasets != NULL);
    return NULL;
}

RORI_ID3_LABEL_TYPE rid3_traverse_tree(
    rid3_Node*    tree,
    rid3_Dataset  request
) {
    assert(tree != NULL);
    return RORI_ID3_LABEL_NOT_FOUND; // Not found or invalid
}

/////////////////
// Internal defs
/////////////////

static float calculate_entropy(rid3_Dataset* datasets, size_t len)
{
    if (datasets == NULL || len == 0) return 0.0f;
    float entropy = 0.0f;

    RORI_ID3_LABEL_TYPE* unique_labels = (RORI_ID3_LABEL_TYPE*)malloc(len * sizeof(RORI_ID3_LABEL_TYPE));
    size_t* counts = (size_t*)malloc(len * sizeof(size_t));
    size_t unique_count = 0;

    for (size_t i = 0; i < len; i++) {
        RORI_ID3_LABEL_TYPE current_label = datasets[i].label;
        int found = 0;

        for (size_t j = 0; j < unique_count; j++) {
            // NOTE : this assumes `RORI_ID3_LABEL_TYPE` is a primitive (int, enum, char).
            if (unique_labels[j] == current_label) {
                counts[j]++;
                found = 1;
                break;
            }
        }

        if (!found) {
            unique_labels[unique_count] = current_label;
            counts[unique_count] = 1;
            unique_count++;
        }
    }

    for (size_t i = 0; i < unique_count; i++) {
        float probability = (float)counts[i] / (float)len;
        // Entropy = -p * log2(p)
        entropy -= probability * log2f(probability); 
    }

    free(unique_labels);
    free(counts);

    return entropy;
}

static float calculate_information_gain(rid3_Dataset* datasets, size_t len)
{
    // TODO : Implementation
    return 0.0f;
}
