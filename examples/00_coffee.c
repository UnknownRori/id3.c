#include <stdio.h>
#include <stdlib.h>
#include "../id3.h"

#define ARR_LEN(ARR) (sizeof(ARR) / sizeof(*ARR))

typedef enum State {
    LOW, NORMAL, HIGH
} State;

typedef enum Attribute {
    TEMP, GUEST_COUNT, WEATHER
} Attribute;

const char* COFFEE_LABEL_NAME[] = {
    "Hot Latte",
    "Iced Latte",
    "Iced Americano",
    "Hot Americano",
    "Decaf Espresso",
};

const char* TEMP_LABEL_NAME[] = {
    "Cold",
    "Normal",
    "Hot",
};

const char* GUEST_COUNT_LABEL_NAME[] = {
    "Sparse",
    "As Usual",
    "Dense",
};

const char* WEATHER_LABEL_NAME[] = {
    "Cloudy",
    "Sunny",
    "Rain",
};

static rid3_Dataset make_dataset(
    RORI_ID3_LABEL_TYPE label,
    RORI_ID3_ATTRIBUTE_TYPE temp,
    RORI_ID3_ATTRIBUTE_TYPE guest_count,
    RORI_ID3_ATTRIBUTE_TYPE weather
);

static void print_dataset(
    rid3_Dataset* dataset
);

static void free_dataset(
    rid3_Dataset* dataset
);

int main()
{
    rid3_Dataset datasets[] = {
        make_dataset(0, LOW     , NORMAL    , LOW   ),
        make_dataset(1, HIGH    , NORMAL    , NORMAL),
        make_dataset(3, LOW     , HIGH      , HIGH  ),
        make_dataset(2, NORMAL  , HIGH      , HIGH  ),
        make_dataset(4, LOW     , LOW       , LOW   ),
        make_dataset(2, HIGH    , HIGH      , HIGH  ),
    };

    printf("total datasets: %zu\n", ARR_LEN(datasets));
    rid3_Node* root = rid3_build_decision_tree(datasets, ARR_LEN(datasets));

    rid3_Dataset prompt = make_dataset(-1, HIGH, HIGH, HIGH);
    RORI_ID3_LABEL_TYPE recommended = rid3_traverse_tree(root, prompt);

    printf("resulting recommendation: %s (%d)", COFFEE_LABEL_NAME[recommended], recommended);

    // Free it like a good practice although OS already clean up after this so whatever
    rid3_free_tree(root);
    for (size_t i = 0; i < ARR_LEN(datasets); i++) {
        free_dataset(&datasets[i]);
    }
    return 0;
}

static rid3_Dataset make_dataset(
    RORI_ID3_LABEL_TYPE label,
    RORI_ID3_ATTRIBUTE_TYPE temp,
    RORI_ID3_ATTRIBUTE_TYPE guest_count,
    RORI_ID3_ATTRIBUTE_TYPE weather
) {
    RORI_ID3_ATTRIBUTE_TYPE* attributes = malloc(sizeof(RORI_ID3_ATTRIBUTE_TYPE) * 3);
    attributes[0] = temp;
    attributes[1] = guest_count;
    attributes[2] = weather;
    return (rid3_Dataset) {
        .label  = label,
        .values = attributes,
        .len    = 3,
    };
}

static void print_dataset(
    rid3_Dataset* dataset
)
{
    RORI_ID3_ATTRIBUTE_TYPE temp        = dataset->values[0];
    RORI_ID3_ATTRIBUTE_TYPE guest_count = dataset->values[1];
    RORI_ID3_ATTRIBUTE_TYPE weather     = dataset->values[2];
    printf("label: %s (%d)\n", COFFEE_LABEL_NAME[dataset->label], dataset->label);
    printf("| raw     : {%d, %d, %d}\n", temp, guest_count, weather);
    printf("| temp    : %s\n", TEMP_LABEL_NAME[temp]);
    printf("| guest   : %s\n", TEMP_LABEL_NAME[guest_count]);
    printf("| weather : %s\n", TEMP_LABEL_NAME[weather]);
}

static void free_dataset(
    rid3_Dataset* dataset
) { free(dataset->values); }
