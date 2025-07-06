#include "arraylist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ArrayList* array_create(size_t element_size) {
    ArrayList *array = malloc(sizeof(ArrayList));
    array->capacity = 0;
    array->count = 0;
    array->element_size = element_size;
    array->items = NULL;
    return array;
}

void array_push(ArrayList *array, void* data) {
    if (array->capacity == 0) {
        array->capacity = 256;
        array->items = malloc(array->element_size * array->capacity);
        if (!array->items) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    } else if (array->capacity == array->count) {
        array->capacity *= 2;
        void *new_items = realloc(array->items, array->element_size * array->capacity);
        if (!new_items) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1);
        }
        array->items = new_items;
    }
    
    // Copy the data
    memcpy((char*)array->items + (array->count * array->element_size), data, array->element_size);
    array->count++;
}

void array_destroy(ArrayList *array) {
    if (array->items) {
        free(array->items);
        array->items = NULL;
    }
    array->capacity = 0;
    array->count = 0;
}

void array_inspect(ArrayList *array) {
    printf("[INFO]: array = (count: %d, capacity: %d)\n", array->count,
           array->capacity);
}

void array_append(ArrayList *array, char *str) {
    if (!str)
        return;

    size_t str_len = strlen(str);
    for (size_t i = 0; i < str_len; i++) {
        array_push(array, &str[i]);
    }
}

void print_array(ArrayList *array) {
    printf("%.*s\n", array->count, (char *)array->items);
}
