#pragma once

#include <stddef.h> 

typedef struct {
    int capacity;
    int count;
    size_t element_size;
    void *items;
} ArrayList;

void array_push(ArrayList *array, void *data);
void array_destroy(ArrayList *array);
void array_inspect(ArrayList *array);
void print_array(ArrayList *array);
ArrayList* array_create(size_t element_size);
