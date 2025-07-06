#pragma once

#include "arraylist.h"
#include "hashmap.h"
#include "thirdparty/jimp.h"

typedef struct {
    void (*print)(void *self);
} JsonObjectBase;

typedef struct {
    void (*print)(void *self);
    Hashmap *map;
} JsonObject;

typedef struct {
    void (*print)(void *self);
    double *number;
} JsonNumber;

typedef struct {
    void (*print)(void *self);
    char *string;
} JsonString;

typedef struct {
    void (*print)(void *self);
    ArrayList *array;
} JsonArray;

typedef struct {
    void (*print)(void *self);
    bool *value;
} JsonBool;

typedef enum { JSON_OBJECT, JSON_NUMBER, JSON_STRING, JSON_ARRAY } JsonType;

JsonObjectBase *json_parse(Jimp *jimp);
