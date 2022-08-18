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

static void free_table(ConfigTable *t) {
    arrayMap(&t->pairs, free_pair_callback, NULL);
    arrayFree(&t->pairs);
    free(t->name);
    free(t);
}

static void free_table_callback(void *table, void *cl) {
    (void)cl; // unused
    ConfigTable *t = (ConfigTable *)table;
    free_table(t);
}

ConfigTable *config_parse(ConfigParser *p, const char *config_file_path) {
    if(!p) {
        errno = EINVAL;
        return NULL;
    }

    p->config_file_path = strdup(config_file_path);

    char *file_contents = read_file(config_file_path);
    if(!file_contents) {
        // errno is set by read_file().
        return NULL;
    }

    arrayInit(&p->tables);
    if(!config_parser_parse(file_contents, &p->tables)) {
        free(file_contents);
        arrayMap(&p->tables, free_table_callback, NULL);
        return NULL;
    }
    free(file_contents);
    // the first table is aways present and is the top-level.
    return ARRAY_GET_AS(ConfigTable *, &p->tables, 0);
}

void config_end(ConfigParser *p) {
    arrayMap(&p->tables, free_table_callback, NULL);
    arrayFree(&p->tables);
    free(p->config_file_path);
}


int config_table_count(ConfigParser *p) {
    return p->tables.used;
}

ConfigTable *config_get_table(ConfigParser *p, const char *name) {
    for(size_t i = 0; i < p->tables.used; ++i) {
        ConfigTable *table = ARRAY_GET_AS(ConfigTable *, &p->tables, i);
        if(!strcmp(table->name, name)) {
            return table;
        }
    }
    errno = EINVAL;
    return NULL;
}

#define MAKE_VALUE(ok_, type, val) ((ConfigValue){.ok = (ok_), .as = {.type = (val)}})
ConfigValue config_get_string(ConfigTable *t, const char *key) {
    for(size_t i = 0; i < t->pairs.used; ++i) {
        Pair *pair = ARRAY_GET_AS(Pair *, &t->pairs, i);
        if(!strcmp(pair->key, key)) {
            if(pair->value.type != LIT_STRING) {
                return MAKE_VALUE(false, number, 0);
            } else {
                return MAKE_VALUE(true, string, pair->value.as.string);
            }
        }
    }
    errno = EINVAL;
    return MAKE_VALUE(false, number, 0);
}

ConfigValue config_get_number(ConfigTable *t, const char *key) {
    for(size_t i = 0; i < t->pairs.used; ++i) {
        Pair *pair = ARRAY_GET_AS(Pair *, &t->pairs, i);
        if(!strcmp(pair->key, key)) {
            if(pair->value.type != LIT_NUMBER) {
                return MAKE_VALUE(false, number, 0);
            } else {
                return MAKE_VALUE(true, number, pair->value.as.number);
            }
        }
    }
    errno = EINVAL;
    return MAKE_VALUE(false, number, 0);
}

ConfigValue config_get_boolean(ConfigTable *t, const char *key) {
    for(size_t i = 0; i < t->pairs.used; ++i) {
        Pair *pair = ARRAY_GET_AS(Pair *, &t->pairs, i);
        if(!strcmp(pair->key, key)) {
            if(pair->value.type != LIT_BOOLEAN) {
                return MAKE_VALUE(false, number, 0);
            } else {
                return MAKE_VALUE(true, boolean, pair->value.as.boolean);
            }
        }
    }
    errno = EINVAL;
    return MAKE_VALUE(false, number, 0);
}
#undef MAKE_VALUE
