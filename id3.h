/*
/////////////////////////////////////////////////////////////////////////////
// file  : id3.h
// author: UnknownRori (unknownrori@proton.me)
// description:
// lorem ipsum
//
/////////////////////////////////////////////////////////////////////////////





*/

#ifndef RORI_ID3_H
#define RORI_ID3_H

#include <stddef.h>
#include <stdbool.h>

#ifndef RORI_ID3_LABEL_TYPE
#   include <stdint.h>
#   define RORI_ID3_LABEL_TYPE      int32_t
#   define RORI_ID3_LABEL_NOT_FOUND -1
#endif

#ifndef RORI_ID3_ATTRIBUTE_TYPE
#   include <stdint.h>
#   define RORI_ID3_ATTRIBUTE_TYPE uint32_t
#endif

#ifndef RORI_ID3_CALLOC
#   include <stdlib.h>
#   define RORI_ID3_CALLOC calloc
#endif

#ifndef RORI_ID3_FREE
#   include <stdlib.h>
#   define RORI_ID3_FREE   free
#endif

typedef struct rid3_Dataset {
    RORI_ID3_LABEL_TYPE label;
    RORI_ID3_ATTRIBUTE_TYPE* values;
    size_t len;
} rid3_Dataset;

typedef struct rid3_Node {
    RORI_ID3_LABEL_TYPE label;
    bool is_leaf;

    RORI_ID3_LABEL_TYPE attr_idx;
    RORI_ID3_ATTRIBUTE_TYPE* edge_values;
    struct rid3_Node* child;
    size_t child_count;
} rid3_Node;

rid3_Node* rid3_build_decision_tree(
    rid3_Dataset* datasets,
    size_t        datasets_len
);

RORI_ID3_LABEL_TYPE rid3_traverse_tree(
    rid3_Node*    tree,
    rid3_Dataset  request
);

#endif  // RORI_ID3_H
