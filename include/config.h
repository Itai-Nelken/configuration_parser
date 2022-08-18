#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h> // size_t
#include <stdint.h>
#include <stdbool.h>
#include "array.h"

// A view into a Literal.
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

typedef struct config_parser {
    Array tables; // Array<ConfigTable *>
    char *config_file_path;
} ConfigParser;

/***
 * Parse a configuration file.
 *
 * @param p An *uninitialized* ConfigParser.
 * @param config_file_path The path to the configuration file.
 * @return A pointer to the top-level table or NULL on failure and errno is set.
 ***/
ConfigTable *config_parse(ConfigParser *p, const char *config_file_path);

/***
 * Free a configuration parser.
 *
 * @param p The ConfigParser to free.
 ***/
void config_end(ConfigParser *p);

/***
 * Return the amount of top-level tables.
 *
 * @param p An initialized ConfigParser.
 * @return The number of top-level tables.
 ***/
int config_table_count(ConfigParser *p);


/***
 * Get a table from a parsed configuration file.
 *
 * @param p An initialized ConfigParser.
 * @param name The name of the table.
 * @return A pointer to the table or NULL on failure and errno is set to EINVAL.
 ***/
ConfigTable *config_get_table(ConfigParser *p, const char *name);

/***
 * Get a string value using 'key' from a table.
 * errno is set to EINVAL if the key isn't found.
 *
 * @param t A ConfigTable.
 * @param key The key to get the value from.
 * @return The value with ok set to true on success, and false on failure.
 ***/
ConfigValue config_get_string(ConfigTable *t, const char *key);

/***
 * Get a number value using 'key' from a table.
 * errno is set to EINVAL if the key isn't found.
 *
 * @param t A ConfigTable.
 * @param key The key to get the value from.
 * @return The value with ok set to true on success, and false on failure.
 ***/
ConfigValue config_get_number(ConfigTable *t, const char *key);

/***
 * Get a boolean value using 'key' from a table.
 * errno is set to EINVAL if the key isn't found.
 *
 * @param t A ConfigTable.
 * @param key The key to get the value from.
 * @return The value with ok set to true on success, and false on failure.
 ***/
ConfigValue config_get_boolean(ConfigTable *t, const char *key);

#endif // CONFIG_H
