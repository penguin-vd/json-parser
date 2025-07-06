# JSON Parser

A JSON parser written in C that provides structured representation of JSON data.

## Supported JSON Types

| JSON Type | C Structure | Description |
|-----------|-------------|-------------|
| Object    | `JsonObject` | Key-value pairs stored in hashmap |
| Array     | `JsonArray` | Ordered list of values |
| String    | `JsonString` | UTF-8 text strings |
| Number    | `JsonNumber` | Double-precision floating point |
| Boolean   | `JsonBoolean` | True/false values |
| Null      | `JsonObjectBase` | Null values |

## Dependencies

### Third-Party Libraries

This parser depends on the following external libraries:

- **[tsoding/nob.h](https://github.com/tsoding/nob.h)** - A header-only build system and utility library
- **[jimp.h](https://github.com/tsoding/jim)** - A minimalist JSON parser library that provides the low-level parsing primitives

### Internal Dependencies

- `arraylist.h` - Dynamic array implementation
- `hashmap.h` - Hash table implementation for object key-value storage

## Installation

1. Clone the repository with dependencies:
```bash
git clone <your-repo-url>
cd json-parser
```

2. Build the project:
```bash
make
```

## Usage

### Basic Parsing

```c
#include "json.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "thirdparty/nob.h"
#define JIMP_IMPLEMENTATION
#include "thirdparty/jimp.h"

int main(void) {
    const char *file_path = "test.json";

    String_Builder sb = {0};
    if (!read_entire_file(file_path, &sb))
        return 1;

    Jimp jimp = {0};

    jimp_begin(&jimp, file_path, sb.items, sb.count);
    JsonObjectBase *root = json_parse(&jimp);

    root->print(root);
    printf("\n");
    return 0;
}
```
