#pragma once

#include "arraylist.h"
#include "hashmap.h"
#include "thirdparty/jimp.h"

typedef enum { JSON_OBJECT, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_BOOLEAN, JSON_NULL } JsonType;
typedef struct {
  char *(*json_encode)(void *self);
  JsonType type;
} JsonObjectBase;

typedef struct {
  char *(*json_encode)(void *self);
  Hashmap *map;
  JsonType type;
} JsonObject;

typedef struct {
  char *(*json_encode)(void *self);
  double *number;
  JsonType type;
} JsonNumber;

typedef struct {
  char *(*json_encode)(void *self);
  char *string;
  JsonType type;
} JsonString;

typedef struct {
  char *(*json_encode)(void *self);
  ArrayList *array;
  JsonType type;
} JsonArray;

typedef struct {
  char *(*json_encode)(void *self);
  bool *value;
  JsonType type;
} JsonBool;

JsonObjectBase *json_parse(Jimp *jimp);
void json_free_object(JsonObjectBase* obj);
