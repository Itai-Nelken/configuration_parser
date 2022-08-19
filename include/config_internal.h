#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h> // size_t
#include <stdint.h>
#include <stdbool.h>
#include "array.h"

// A view into a Literal.
// change version in config_parser.h when changing here.
typedef struct config_value {
    bool ok;
    union {
        int64_t number;
        bool boolean;
        char *string;
    } as;
} ConfigValue;

typedef struct config_table {
    char *name;
    Array pairs; // Array<Pair *>
} ConfigTable;

// change version in config_parser.h when changing here.
typedef struct config_parser {
    Array *tables; // Array<ConfigTable *>
    char *config_file_path;
} ConfigParser;

#endif // CONFIG_H
