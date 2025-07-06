#pragma once
#include "arraylist.h"

typedef struct HashNode {
  char *key;
  void *value;
  struct HashNode *next;
} HashNode;

typedef struct {
  int capacity;
  int count;
  HashNode **buckets;
  ArrayList keys;
} Hashmap;

void init_hashmap(Hashmap *h, int capacity);
void destroy_hashmap(Hashmap *h);
void hashmap_set(Hashmap *h, char *key, void *value);
void *hashmap_get(Hashmap *h, char *key);
void hashmap_print(Hashmap *h);
int hashmap_has_key(Hashmap *h, char *key);
void hashmap_remove(Hashmap *h, char *key);
ArrayList *hashmap_get_keys_list(Hashmap *h);
int hashmap_size(Hashmap *h);
int string_hash(char *str, int count);
void hashmap_print_json(Hashmap *h);
