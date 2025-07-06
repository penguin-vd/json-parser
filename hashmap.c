#include "hashmap.h"
#include "arraylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_hashmap(Hashmap *h, int capacity) {
    h->capacity = capacity;
    h->count = 0;
    h->buckets = calloc(capacity, sizeof(HashNode *));
    if (!h->buckets) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    h->keys = (ArrayList){0};
    h->keys.element_size = sizeof(char *);
}

void destroy_hashmap(Hashmap *h) {
    if (!h)
        return;

    for (int i = 0; i < h->capacity; i++) {
        HashNode *current = h->buckets[i];
        while (current) {
            HashNode *next = current->next;
            free(current->key);

            free(current);
            current = next;
        }
    }

    char **keys_array = (char **)h->keys.items;
    for (int i = 0; i < h->keys.count; i++) {
        free(keys_array[i]);
    }
    array_destroy(&h->keys);

    free(h->buckets);
    h->capacity = 0;
    h->count = 0;
}

static void add_key_to_list(Hashmap *h, char *key) {

    char *key_copy = malloc(strlen(key) + 1);
    if (!key_copy) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(key_copy, key);
    array_push(&h->keys, &key_copy);
}

static void remove_key_from_list(Hashmap *h, char *key) {

    char **keys_array = (char **)h->keys.items;
    for (int i = 0; i < h->keys.count; i++) {
        if (strcmp(keys_array[i], key) == 0) {
            free(keys_array[i]);

            for (int j = i; j < h->keys.count - 1; j++) {
                keys_array[j] = keys_array[j + 1];
            }
            h->keys.count--;
            break;
        }
    }
}

void hashmap_set(Hashmap *h, char *key, void *value) {
    if (!h || !key)
        return;

    int hash = string_hash(key, strlen(key));
    int bucket_index = hash % h->capacity;

    HashNode *current = h->buckets[bucket_index];

    while (current) {
        if (strcmp(current->key, key) == 0) {

            current->value = value;
            return;
        }
        current = current->next;
    }

    HashNode *new_node = malloc(sizeof(HashNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        fprintf(stderr, "Memory allocation failed\n");
        free(new_node);
        exit(1);
    }
    strcpy(new_node->key, key);

    new_node->value = value;
    new_node->next = h->buckets[bucket_index];
    h->buckets[bucket_index] = new_node;

    add_key_to_list(h, key);
    h->count++;
}

void *hashmap_get(Hashmap *h, char *key) {
    if (!h || !key)
        return NULL;

    int hash = string_hash(key, strlen(key));
    int bucket_index = hash % h->capacity;

    HashNode *current = h->buckets[bucket_index];

    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

int hashmap_has_key(Hashmap *h, char *key) {
    return hashmap_get(h, key) != NULL;
}

void hashmap_remove(Hashmap *h, char *key) {
    if (!h || !key)
        return;

    int hash = string_hash(key, strlen(key));
    int bucket_index = hash % h->capacity;

    HashNode *current = h->buckets[bucket_index];
    HashNode *prev = NULL;

    while (current) {
        if (strcmp(current->key, key) == 0) {

            if (prev) {
                prev->next = current->next;
            } else {
                h->buckets[bucket_index] = current->next;
            }

            remove_key_from_list(h, key);
            h->count--;

            free(current->key);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

ArrayList *hashmap_get_keys_list(Hashmap *h) {
    if (!h)
        return NULL;
    return &h->keys;
}

int hashmap_size(Hashmap *h) {
    if (!h)
        return 0;
    return h->count;
}

int string_hash(char *str, int count) {
    int hash = 0;
    for (int i = 0; i < count; i++) {
        hash = hash * 31 + str[i];
    }
    return hash < 0 ? -hash : hash;
}
