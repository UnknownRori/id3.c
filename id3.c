#include <assert.h>
#include <math.h>
#include "id3.h"

static float calculate_entropy(rid3_Dataset* datasets, size_t len);
static float calculate_information_gain(rid3_Dataset* datasets, size_t len, RORI_ID3_LABEL_TYPE attr_idx);
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

    RORI_ID3_LABEL_TYPE* unique_labels = (RORI_ID3_LABEL_TYPE*)RORI_ID3_CALLOC(len, sizeof(RORI_ID3_LABEL_TYPE));
    size_t* counts = (size_t*)RORI_ID3_CALLOC(len, sizeof(size_t));
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

    RORI_ID3_FREE(unique_labels);
    RORI_ID3_FREE(counts);

    return entropy;
}

static float calculate_information_gain(rid3_Dataset* datasets, size_t len, RORI_ID3_LABEL_TYPE attr_idx)
{
    if (datasets == NULL || len == 0) return 0.0f;

    float base_entropy = calculate_entropy(datasets, len);

    RORI_ID3_ATTRIBUTE_TYPE* unique_vals = (RORI_ID3_ATTRIBUTE_TYPE*)RORI_ID3_CALLOC(len, sizeof(RORI_ID3_ATTRIBUTE_TYPE));
    size_t* val_counts = (size_t*)RORI_ID3_CALLOC(len, sizeof(size_t));
    size_t unique_count = 0;

    for (size_t i = 0; i < len; i++) {
        RORI_ID3_ATTRIBUTE_TYPE current_val = datasets[i].values[attr_idx];
        int found = 0;

        for (size_t j = 0; j < unique_count; j++) {
            // NOTE : this assumes `RORI_ID3_LABEL_TYPE` is a primitive (int, enum, char).
            if (unique_vals[j] == current_val) {
                val_counts[j]++;
                found = 1;
                break;
            }
        }

        if (!found) {
            unique_vals[unique_count] = current_val;
            val_counts[unique_count] = 1;
            unique_count++;
        }
    }
    float weighted_subset_entropy = 0.0f;

    for (size_t v = 0; v < unique_count; v++) {
        size_t subset_size = val_counts[v];
        RORI_ID3_ATTRIBUTE_TYPE target_val = unique_vals[v];

        // TODO : A bit bad to allocate memory space in each iteration
        rid3_Dataset* subset = (rid3_Dataset*)RORI_ID3_CALLOC(subset_size, sizeof(rid3_Dataset));
        size_t subset_idx = 0;

        for (size_t i = 0; i < len; i++) {
            if (datasets[i].values[attr_idx] == target_val) {
                subset[subset_idx++] = datasets[i];
            }
        }

        float sub_entropy = calculate_entropy(subset, subset_size);

        float weight = (float)subset_size / (float)len;
        weighted_subset_entropy += weight * sub_entropy;

        RORI_ID3_FREE(subset);
    }

    RORI_ID3_FREE(unique_vals);
    RORI_ID3_FREE(val_counts);
    return base_entropy - weighted_subset_entropy;
}
