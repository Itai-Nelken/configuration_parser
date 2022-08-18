#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strdup
#include <errno.h>
#include <stdbool.h>
#include "array.h"
#include "parser.h"
#include "config.h"

// sets errno.
// the returned buffer has to be freed (with free()).
static char *read_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if(!fp) {
        // errno is set by fopen().
        return NULL;
    }

    // FIXME: This isn't portable. SEEK_END is optional according to posix.
    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    rewind(fp);

    char *buffer = calloc(length + 1, sizeof(char));
    if(!buffer) {
        // errno is set by calloc().
        fclose(fp);
        return false;
    }
    // TODO: check that fread read 'length' bytes.
    fread(buffer, sizeof(char), length, fp);
    buffer[length] = '\0';

    fclose(fp);
    return buffer;
}

static void free_pair_callback(void *pair, void *cl) {
    (void)cl; // unused
    Pair *p = (Pair *)pair;
    free_pair(p);
}

bool config_init(ConfigParser *p, const char *config_file_path) {
    if(!p) {
        errno = EINVAL;
        return false;
    }

    p->config_file_path = strdup(config_file_path);

    char *file_contents = read_file(config_file_path);
    if(!file_contents) {
        // errno is set by read_file().
        return false;
    }

    arrayInit(&p->pairs);

    if(!config_parser_parse(file_contents, &p->pairs)) {
        free(file_contents);
        return false;
    }
    free(file_contents);
    return true;
}

void config_end(ConfigParser *p) {
    arrayMap(&p->pairs, free_pair_callback, NULL);
    arrayFree(&p->pairs);
    free(p->config_file_path);
}

char *config_get_string(ConfigParser *p, const char *key) {
    for(size_t i = 0; i < p->pairs.used; ++i) {
        Pair *pair = ARRAY_GET_AS(Pair *, &p->pairs, i);
        if(!strcmp(pair->key, key)) {
            return pair->value.type == LIT_STRING ? pair->value.as.string : NULL;
        }
    }
    errno = EINVAL;
    return NULL;
}
