#include "json.h"
#include "thirdparty/jimp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void json_object_print(void *self);
static void json_number_print(void *self);
static void json_string_print(void *self);
static void json_array_print(void *self);
static void json_bool_print(void *self);
static void json_null_print(void *self);

static JsonObject *create_json_object(Hashmap *map) {
    JsonObject *obj = malloc(sizeof(JsonObject));
    if (!obj) {
        fprintf(stderr, "Memory allocation failed for JsonObject\n");
        exit(1);
    }
    obj->print = json_object_print;
    obj->map = map;
    return obj;
}

static JsonNumber *create_json_number(double value) {
    JsonNumber *num = malloc(sizeof(JsonNumber));
    if (!num) {
        fprintf(stderr, "Memory allocation failed for JsonNumber\n");
        exit(1);
    }
    num->print = json_number_print;
    num->number = malloc(sizeof(double));
    if (!num->number) {
        free(num);
        fprintf(stderr, "Memory allocation failed for double\n");
        exit(1);
    }
    *(num->number) = value;
    return num;
}

static JsonString *create_json_string(const char *value) {
    JsonString *str = malloc(sizeof(JsonString));
    if (!str) {
        fprintf(stderr, "Memory allocation failed for JsonString\n");
        exit(1);
    }
    str->print = json_string_print;
    str->string = malloc(strlen(value) + 1);
    if (!str->string) {
        free(str);
        fprintf(stderr, "Memory allocation failed for string\n");
        exit(1);
    }
    strcpy(str->string, value);
    return str;
}

static JsonArray *create_json_array(ArrayList *array) {
    JsonArray *arr = malloc(sizeof(JsonArray));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed for JsonArray\n");
        exit(1);
    }
    arr->print = json_array_print;
    arr->array = array;
    return arr;
}

static JsonBool *create_json_bool(bool value) {
    JsonBool *num = malloc(sizeof(JsonBool));
    if (!num) {
        fprintf(stderr, "Memory allocation failed for JsonNumber\n");
        exit(1);
    }
    num->print = json_bool_print;
    num->value = malloc(sizeof(bool));
    if (!num->value) {
        free(num);
        fprintf(stderr, "Memory allocation failed for JsonBool\n");
        exit(1);
    }
    *(num->value) = value;
    return num;
}
static JsonObjectBase *create_json_null() {
    JsonObjectBase *arr = malloc(sizeof(JsonObjectBase));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed for JsonObjectBase\n");
        exit(1);
    }
    arr->print = json_null_print;
    return arr;
}

static JsonObjectBase *parse_json_value(Jimp *jimp);

static ArrayList *parse_json_array(Jimp *jimp) {
    ArrayList *list = array_create(sizeof(JsonObjectBase *));
    if (!list) {
        fprintf(stderr, "Memory allocation failed for ArrayList\n");
        exit(1);
    }

    while (jimp_array_item(jimp)) {
        JsonObjectBase *value = parse_json_value(jimp);
        if (value) {
            array_push(list, &value);
        }
    }

    return list;
}

static Hashmap *parse_json_object(Jimp *jimp) {
    Hashmap *map = malloc(sizeof(Hashmap));
    if (!map) {
        fprintf(stderr, "Memory allocation failed for Hashmap\n");
        exit(1);
    }
    init_hashmap(map, 16);

    while (jimp_object_member(jimp)) {
        char *key = malloc(strlen(jimp->string) + 1);
        if (!key) {
            fprintf(stderr, "Memory allocation failed for key\n");
            exit(1);
        }
        strcpy(key, jimp->string);

        JsonObjectBase *value = parse_json_value(jimp);
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

static JsonObjectBase *parse_json_value(Jimp *jimp) {
    if (jimp_is_number_ahead(jimp)) {
        if (!jimp_number(jimp)) {
            return NULL;
        }
        return (JsonObjectBase *)create_json_number(jimp->number);

    } else if (jimp_is_string_ahead(jimp)) {
        if (!jimp_string(jimp)) {
            return NULL;
        }
        return (JsonObjectBase *)create_json_string(jimp->string);

    } else if (jimp_is_bool_ahead(jimp)) {
        if (!jimp_bool(jimp))
            return NULL;

        return (JsonObjectBase *)create_json_bool(jimp->boolean);

    } else if (jimp_is_null_ahead(jimp)) {
        jimp__get_token(jimp);
        return create_json_null();
    } else if (jimp_is_object_ahead(jimp)) {
        if (!jimp_object_begin(jimp)) {
            return NULL;
        }

        Hashmap *map = parse_json_object(jimp);

        if (!jimp_object_end(jimp)) {
            free(map);
            return NULL;
        }

        return (JsonObjectBase *)create_json_object(map);

    } else if (jimp_is_array_ahead(jimp)) {
        if (!jimp_array_begin(jimp)) {
            return NULL;
        }

        ArrayList *array = parse_json_array(jimp);

        if (!jimp_array_end(jimp)) {
            free(array);
            return NULL;
        }

        return (JsonObjectBase *)create_json_array(array);
    }

    return NULL;
}

JsonObjectBase *json_parse(Jimp *jimp) { return parse_json_value(jimp); }

static void json_object_print(void *self) {
    JsonObject *obj = (JsonObject *)self;
    printf("{\n");
    for (int i = 0; i < obj->map->keys.count; i++) {
        char *key = ((char **)obj->map->keys.items)[i];
        printf("\"%s\": ", key);

        JsonObjectBase *base = (JsonObjectBase *)hashmap_get(obj->map, key);
        base->print(base);
        if (i == obj->map->keys.count - 1) {
            printf("\n");
        } else {
            printf(",\n");
        }
    }

    printf("}");
}

static void json_number_print(void *self) {
    JsonNumber *num = (JsonNumber *)self;
    printf("%.6f", *(num->number));
}

static void json_string_print(void *self) {
    JsonString *str = (JsonString *)self;
    printf("\"%s\"", str->string);
}

static void json_array_print(void *self) {
    JsonArray *arr = (JsonArray *)self;
    printf("[");
    if (arr->array && arr->array->count > 0) {
        JsonObjectBase **items = (JsonObjectBase **)arr->array->items;
        for (int i = 0; i < arr->array->count; i++) {
            if (i > 0)
                printf(", ");
            if (items[i]) {
                items[i]->print(items[i]);
            }
        }
    }
    printf("]");
}

static void json_bool_print(void *self) {
    JsonBool *b = (JsonBool *)self;
    if (*b->value) {
        printf("true");
    } else {
        printf("false");
    }

}
static void json_null_print(void *self) { printf("null"); }
