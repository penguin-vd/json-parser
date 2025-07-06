#include "json.h"
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "thirdparty/nob.h"
#define JIMP_IMPLEMENTATION
#include "thirdparty/jimp.h"

int main(void) {
    const char* file_path = "test.json";

    String_Builder sb = {0};
    if (!read_entire_file(file_path, &sb))
        return 1;

    Jimp jimp = {0};

    jimp_begin(&jimp, file_path, sb.items, sb.count);
    JsonObjectBase* root = json_parse(&jimp);

    char* json = root->json_encode(root);
    printf("%s\n", json);
    free(json);

    json_free_object(root);
    nob_sb_free(sb);
    return 0;
}
