#include "json.h"
#include "arraylist.h"
#include "hashmap.h"
#include "thirdparty/jimp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* json_object_encode(void* self);
static char* json_number_encode(void* self);
static char* json_string_encode(void* self);
static char* json_array_encode(void* self);
static char* json_bool_encode(void* self);
static char* json_null_encode(void* self);

static JsonObject* create_json_object(Hashmap* map) {
    JsonObject* obj = malloc(sizeof(JsonObject));
    if (!obj) {
        fprintf(stderr, "Memory allocation failed for JsonObject\n");
        exit(1);
    }
    obj->json_encode = json_object_encode;
    obj->map = map;
    obj->type = JSON_OBJECT;
    return obj;
}

static JsonNumber* create_json_number(double value) {
    JsonNumber* num = malloc(sizeof(JsonNumber));
    if (!num) {
        fprintf(stderr, "Memory allocation failed for JsonNumber\n");
        exit(1);
    }
    num->json_encode = json_number_encode;
    num->number = malloc(sizeof(double));
    if (!num->number) {
        free(num);
        fprintf(stderr, "Memory allocation failed for double\n");
        exit(1);
    }
    *(num->number) = value;
    num->type = JSON_NUMBER;
    return num;
}

static JsonString* create_json_string(const char* value) {
    JsonString* str = malloc(sizeof(JsonString));
    if (!str) {
        fprintf(stderr, "Memory allocation failed for JsonString\n");
        exit(1);
    }
    str->json_encode = json_string_encode;
    str->string = malloc(strlen(value) + 1);
    str->type = JSON_STRING;
    if (!str->string) {
        free(str);
        fprintf(stderr, "Memory allocation failed for string\n");
        exit(1);
    }
    strcpy(str->string, value);
    return str;
}

static JsonArray* create_json_array(ArrayList* array) {
    JsonArray* arr = malloc(sizeof(JsonArray));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed for JsonArray\n");
        exit(1);
    }
    arr->json_encode = json_array_encode;
    arr->array = array;
    arr->type = JSON_ARRAY;
    return arr;
}

static JsonBool* create_json_bool(bool value) {
    JsonBool* b = malloc(sizeof(JsonBool));
    if (!b) {
        fprintf(stderr, "Memory allocation failed for JsonNumber\n");
        exit(1);
    }
    b->json_encode = json_bool_encode;
    b->value = malloc(sizeof(bool));
    b->type = JSON_BOOLEAN;
    if (!b->value) {
        free(b);
        fprintf(stderr, "Memory allocation failed for JsonBool\n");
        exit(1);
    }
    *(b->value) = value;
    return b;
}
static JsonObjectBase* create_json_null() {
    JsonObjectBase* null = malloc(sizeof(JsonObjectBase));
    if (!null) {
        fprintf(stderr, "Memory allocation failed for JsonObjectBase\n");
        exit(1);
    }
    null->json_encode = json_null_encode;
    null->type = JSON_NULL;
    return null;
}

static JsonObjectBase* parse_json_value(Jimp* jimp);

static ArrayList* parse_json_array(Jimp* jimp) {
    ArrayList* list = array_create(sizeof(JsonObjectBase*));
    if (!list) {
        fprintf(stderr, "Memory allocation failed for ArrayList\n");
        exit(1);
    }

    while (jimp_array_item(jimp)) {
        JsonObjectBase* value = parse_json_value(jimp);
        if (value) {
            array_push(list, &value);
        }
    }

    return list;
}

static Hashmap* parse_json_object(Jimp* jimp) {
    Hashmap* map = malloc(sizeof(Hashmap));
    if (!map) {
        fprintf(stderr, "Memory allocation failed for Hashmap\n");
        exit(1);
    }
    init_hashmap(map, 16);

    while (jimp_object_member(jimp)) {
        char* key = malloc(strlen(jimp->string) + 1);
        if (!key) {
            fprintf(stderr, "Memory allocation failed for key\n");
            exit(1);
        }
        strcpy(key, jimp->string);

        JsonObjectBase* value = parse_json_value(jimp);
        if (value) {
            hashmap_set(map, key, value);
        } else {
            fprintf(stderr, "Failed to parse JSON value with key: %s\n", key);
            free(key);
            exit(1);
        }
    }

    return map;
}

static JsonObjectBase* parse_json_value(Jimp* jimp) {
    if (jimp_is_number_ahead(jimp)) {
        if (!jimp_number(jimp)) {
            return NULL;
        }
        return (JsonObjectBase*)create_json_number(jimp->number);

    } else if (jimp_is_string_ahead(jimp)) {
        if (!jimp_string(jimp)) {
            return NULL;
        }
        return (JsonObjectBase*)create_json_string(jimp->string);

    } else if (jimp_is_bool_ahead(jimp)) {
        if (!jimp_bool(jimp))
            return NULL;

        return (JsonObjectBase*)create_json_bool(jimp->boolean);

    } else if (jimp_is_null_ahead(jimp)) {
        jimp__get_token(jimp);
        return create_json_null();
    } else if (jimp_is_object_ahead(jimp)) {
        if (!jimp_object_begin(jimp)) {
            return NULL;
        }

        Hashmap* map = parse_json_object(jimp);

        if (!jimp_object_end(jimp)) {
            free(map);
            return NULL;
        }

        return (JsonObjectBase*)create_json_object(map);

    } else if (jimp_is_array_ahead(jimp)) {
        if (!jimp_array_begin(jimp)) {
            return NULL;
        }

        ArrayList* array = parse_json_array(jimp);

        if (!jimp_array_end(jimp)) {
            free(array);
            return NULL;
        }

        return (JsonObjectBase*)create_json_array(array);
    }

    return NULL;
}

JsonObjectBase* json_parse(Jimp* jimp) { return parse_json_value(jimp); }

static char* json_object_encode(void* self) {
    JsonObject* obj = (JsonObject*)self;

    size_t buffer_size = 2;
    for (int i = 0; i < obj->map->keys.count; i++) {
        char* key = ((char**)obj->map->keys.items)[i];
        buffer_size += strlen(key) + 4;

        JsonObjectBase* base = (JsonObjectBase*)hashmap_get(obj->map, key);
        char* value_str = base->json_encode(base);
        if (value_str) {
            buffer_size += strlen(value_str);
            free(value_str);
        }

        if (i < obj->map->keys.count - 1) {
            buffer_size += 2;
        }
    }

    char* result = malloc(buffer_size + 1);
    if (!result)
        return NULL;

    strcpy(result, "{");
    for (int i = 0; i < obj->map->keys.count; i++) {
        char* key = ((char**)obj->map->keys.items)[i];
        char temp[256];
        snprintf(temp, sizeof(temp), "\"%s\": ", key);
        strcat(result, temp);

        JsonObjectBase* base = (JsonObjectBase*)hashmap_get(obj->map, key);
        char* value_str = base->json_encode(base);
        if (value_str) {
            strcat(result, value_str);
            free(value_str);
        }

        if (i < obj->map->keys.count - 1) {
            strcat(result, ", ");
        }
    }
    strcat(result, "}");

    return result;
}

static char* json_number_encode(void* self) {
    JsonNumber* num = (JsonNumber*)self;
    char* result = malloc(32);
    if (!result)
        return NULL;

    snprintf(result, 32, "%.6f", *(num->number));
    return result;
}

static char* json_string_encode(void* self) {
    JsonString* str = (JsonString*)self;
    size_t len = strlen(str->string) + 3;
    char* result = malloc(len);
    if (!result)
        return NULL;

    snprintf(result, len, "\"%s\"", str->string);
    return result;
}

static char* json_array_encode(void* self) {
    JsonArray* arr = (JsonArray*)self;

    size_t buffer_size = 2;
    if (arr->array && arr->array->count > 0) {
        JsonObjectBase** items = (JsonObjectBase**)arr->array->items;
        for (int i = 0; i < arr->array->count; i++) {
            if (i > 0)
                buffer_size += 2;
            if (items[i]) {
                char* item_str = items[i]->json_encode(items[i]);
                if (item_str) {
                    buffer_size += strlen(item_str);
                    free(item_str);
                }
            }
        }
    }

    char* result = malloc(buffer_size + 1);
    if (!result)
        return NULL;

    strcpy(result, "[");
    if (arr->array && arr->array->count > 0) {
        JsonObjectBase** items = (JsonObjectBase**)arr->array->items;
        for (int i = 0; i < arr->array->count; i++) {
            if (i > 0)
                strcat(result, ", ");
            if (items[i]) {
                char* item_str = items[i]->json_encode(items[i]);
                if (item_str) {
                    strcat(result, item_str);
                    free(item_str);
                }
            }
        }
    }
    strcat(result, "]");

    return result;
}

static char* json_bool_encode(void* self) {
    JsonBool* b = (JsonBool*)self;
    char* result = malloc(6);
    if (!result)
        return NULL;

    if (*b->value) {
        strcpy(result, "true");
    } else {
        strcpy(result, "false");
    }
    return result;
}

static char* json_null_encode(void* self) {
    char* result = malloc(5);
    if (!result)
        return NULL;
    strcpy(result, "null");
    return result;
}

void json_free_object(JsonObjectBase* base) {
    switch (base->type) {
        case JSON_OBJECT: {
            JsonObject* obj = (JsonObject*)base;
            for (int i = 0; i < obj->map->keys.count; i++) {
                json_free_object(hashmap_get(obj->map, ((char**)obj->map->keys.items)[i]));
            }
            destroy_hashmap(obj->map);
            free(obj->map);
            free(obj);
        } break;
        case JSON_NUMBER: {
            JsonNumber* num = (JsonNumber*)base;
            free(num->number);
            free(num);
        } break;
        case JSON_STRING: {
            JsonString* str = (JsonString*)base;
            free(str->string);
            free(str);
        } break;
        case JSON_ARRAY: {
            JsonArray* arr = (JsonArray*)base;
            for (int i = 0; i < arr->array->count; i++) {
                JsonObjectBase** items = (JsonObjectBase**)arr->array->items;
                json_free_object(items[i]);
            }
            array_destroy(arr->array);
            free(arr->array);
            free(arr);
        } break;
        case JSON_BOOLEAN: {
            JsonBool* b = (JsonBool*)base;
            free(b->value);
            free(b);
        } break;
        case JSON_NULL:
            free(base);
            break;
    }
}
