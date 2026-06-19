#include <assert.h>
#include <math.h>
#include "id3.h"

static float calculate_entropy(rid3_Dataset* datasets, size_t len);
static float calculate_information_gain(rid3_Dataset* datasets, size_t len, RORI_ID3_LABEL_TYPE attr_idx);
static bool  _is_pure(rid3_Dataset* datasets, size_t len);
static RORI_ID3_LABEL_TYPE  _get_most_common_label(rid3_Dataset* datasets, size_t len);

static rid3_Node* __rid3_build_decision_tree(
    rid3_Dataset*   datasets,
    size_t          datasets_len,
    size_t*         attributes,
    size_t          attributes_len
);

rid3_Node* rid3_build_decision_tree(
    rid3_Dataset* datasets,
    size_t        datasets_len
) {
    assert(datasets != NULL);
    if (datasets_len == 0) return NULL;

    size_t total_attrs = datasets[0].len;
    size_t* available_attrs = (size_t*)RORI_ID3_CALLOC(total_attrs, sizeof(size_t));
    for (size_t i = 0; i < total_attrs; i++) {
        available_attrs[i] = i;
    }

    rid3_Node* root = __rid3_build_decision_tree(datasets, datasets_len, available_attrs, total_attrs);
    RORI_ID3_FREE(available_attrs);

    return root;
}

RORI_ID3_LABEL_TYPE rid3_traverse_tree(
    rid3_Node*    tree,
    rid3_Dataset  request
) {
    assert(tree != NULL);
    if (tree->is_leaf) {
        return tree->label;
    }

    size_t attr_index = tree->attr_idx;
    RORI_ID3_ATTRIBUTE_TYPE request_val = request.values[attr_index];
    for (size_t i = 0; i < tree->child_count; i++) {
        if (tree->edge_values[i] == request_val) {
            return rid3_traverse_tree(tree->child[i], request);
        }
    }

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

static bool  _is_pure(rid3_Dataset* datasets, size_t len)
{
    if (len <= 1) return true;

    RORI_ID3_LABEL_TYPE first_label = datasets[0].label;

    for (size_t i = 1; i < len; i++) {
        // NOTE : this assumes `RORI_ID3_LABEL_TYPE` is a primitive (int, enum, char).
        if (datasets[i].label != first_label) {
            return false;
        }
    }

    return true;
}
static RORI_ID3_LABEL_TYPE  _get_most_common_label(rid3_Dataset* datasets, size_t len)
{
    if (len == 0) return RORI_ID3_LABEL_NOT_FOUND; 
    
    if (len == 1) return datasets[0].label;

    RORI_ID3_LABEL_TYPE* unique_labels = (RORI_ID3_LABEL_TYPE*)RORI_ID3_CALLOC(len, sizeof(RORI_ID3_LABEL_TYPE));
    size_t* counts = (size_t*)RORI_ID3_CALLOC(len, sizeof(size_t));
    size_t unique_count = 0;

    for (size_t i = 0; i < len; i++) {
        RORI_ID3_LABEL_TYPE current_label = datasets[i].label;
        bool found = false;

        for (size_t j = 0; j < unique_count; j++) {
            if (unique_labels[j] == current_label) {
                counts[j]++;
                found = true;
                break;
            }
        }

        if (!found) {
            unique_labels[unique_count] = current_label;
            counts[unique_count] = 1;
            unique_count++;
        }
    }

    size_t max_count = 0;
    size_t best_index = 0;

    for (size_t i = 0; i < unique_count; i++) {
        if (counts[i] > max_count) {
            max_count = counts[i];
            best_index = i;
        }
    }

    RORI_ID3_LABEL_TYPE winning_label = unique_labels[best_index];

    RORI_ID3_FREE(unique_labels);
    RORI_ID3_FREE(counts);

    return winning_label;
}

static rid3_Node* __rid3_build_decision_tree(
    rid3_Dataset*   datasets,
    size_t          datasets_len,
    size_t*         attributes,
    size_t          attributes_len
) {
    if (_is_pure(datasets, datasets_len)) {
        rid3_Node* leaf = (rid3_Node*)RORI_ID3_CALLOC(1, sizeof(rid3_Node));
        leaf->is_leaf       = true;
        leaf->label         = datasets[0].label;
        leaf->edge_values   = NULL;
        leaf->child_count   = 0;
        leaf->child         = NULL;
        return leaf;
    }

    if (attributes_len == 0) {
        rid3_Node* leaf = (rid3_Node*)RORI_ID3_CALLOC(1, sizeof(rid3_Node));
        leaf->is_leaf       = 1;
        leaf->label         = _get_most_common_label(datasets, datasets_len);
        leaf->child_count   = 0;
        leaf->edge_values   = NULL;
        leaf->child         = NULL;
        return leaf;
    }

    size_t best_attr_idx = attributes[0];
    float max_gain = -1.0f;
    size_t attr_array_pos = 0;

    for (size_t i = 0; i < attributes_len; i++) {
        float gain = calculate_information_gain(datasets, datasets_len, attributes[i]);
        if (gain > max_gain) {
            max_gain = gain;
            best_attr_idx = attributes[i];
            attr_array_pos = i;
        }
    }

    rid3_Node* branch = (rid3_Node*)RORI_ID3_CALLOC(1, sizeof(rid3_Node));
    branch->is_leaf  = 0;
    branch->attr_idx = best_attr_idx;

    size_t* remaining_attrs = (size_t*)malloc((attributes_len - 1) * sizeof(size_t));
    size_t r_idx = 0;
    for (size_t i = 0; i < attributes_len; i++) {
        if (i != attr_array_pos) {
            remaining_attrs[r_idx++] = attributes[i];
        }
    }

    RORI_ID3_ATTRIBUTE_TYPE* unique_vals = (RORI_ID3_ATTRIBUTE_TYPE*)RORI_ID3_CALLOC(datasets_len, sizeof(RORI_ID3_ATTRIBUTE_TYPE));
    size_t* val_counts = (size_t*)RORI_ID3_CALLOC(datasets_len, sizeof(size_t));
    size_t unique_count = 0;

    for (size_t i = 0; i < datasets_len; i++) {
        RORI_ID3_ATTRIBUTE_TYPE current_val = datasets[i].values[best_attr_idx];
        int found = 0;
        for (size_t j = 0; j < unique_count; j++) {
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
    branch->child_count = unique_count;
    branch->edge_values = (RORI_ID3_ATTRIBUTE_TYPE*)malloc(unique_count * sizeof(RORI_ID3_ATTRIBUTE_TYPE));
    branch->child = (rid3_Node**)RORI_ID3_CALLOC(unique_count, sizeof(rid3_Node*));

    rid3_Dataset* subset_buffer = (rid3_Dataset*)RORI_ID3_CALLOC(datasets_len, sizeof(rid3_Dataset));
    size_t* insert_indices = (size_t*)RORI_ID3_CALLOC(unique_count, sizeof(size_t));
    size_t current_offset = 0;

    for (size_t v = 0; v < unique_count; v++) {
        insert_indices[v] = current_offset;
        current_offset += val_counts[v];
    }

    for (size_t i = 0; i < datasets_len; i++) {
        RORI_ID3_ATTRIBUTE_TYPE current_val = datasets[i].values[best_attr_idx];
        for (size_t v = 0; v < unique_count; v++) {
            if (unique_vals[v] == current_val) {
                size_t insert_pos = insert_indices[v]++;
                subset_buffer[insert_pos] = datasets[i];
                break;
            }
        }
    }
    size_t read_offset = 0;
    for (size_t v = 0; v < unique_count; v++) {
        branch->edge_values[v] = unique_vals[v];
        
        size_t subset_size = val_counts[v];
        rid3_Dataset* subset_ptr = &subset_buffer[read_offset];

        branch->child[v] = __rid3_build_decision_tree(
            subset_ptr, subset_size, remaining_attrs, attributes_len - 1
        );

        read_offset += subset_size;
    }

    RORI_ID3_FREE(remaining_attrs);
    RORI_ID3_FREE(unique_vals);
    RORI_ID3_FREE(val_counts);
    RORI_ID3_FREE(subset_buffer);
    RORI_ID3_FREE(insert_indices);

    return branch;
}
