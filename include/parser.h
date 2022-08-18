#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include "array.h"

typedef enum literal_type {
    LIT_NONE,
    LIT_NUMBER,
    LIT_BOOLEAN,
    LIT_STRING
} LiteralType;

typedef struct literal {
    LiteralType type;
    union {
        int64_t number;
        bool boolean;
        char *string;
    } as;
} Literal;

typedef struct config_pair {
    char *key;
    Literal value;
} Pair;

void free_literal(Literal l);

void free_pair(Pair *p);

/***
 * Populate a pair array from a config file source.
 *
 * @param source The contents of a configuration file.
 * @param pairs A pointer to the table array to populate.
 * @return true on success, false on failure.
 ***/
bool config_parser_parse(char *source, Array *tables);

#endif // CONFIG_PARSER_H
