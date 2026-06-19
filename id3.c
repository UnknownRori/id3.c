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
    return -1; // Not found or invalid
}

/////////////////
// Internal defs
/////////////////

static float calculate_entropy(rid3_Dataset* datasets, size_t len)
{
    // TODO : Implementations
    return 0.0f;
}

static float calculate_information_gain(rid3_Dataset* datasets, size_t len)
{
    // TODO : Implementation
    return 0.0f;
}
