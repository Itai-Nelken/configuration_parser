#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h> // size_t
#include <stdbool.h>
#include "array.h"

typedef struct config_parser {
    Array pairs; // Array<Pair *>
    char *config_file_path;
} ConfigParser;

/***
 * Initialize a configuration parser.
 * NOTE: The configuration file is read and parsed.
 *
 * @param p The ConfigParser to initialize.
 * @param config_file_path The path to the configuration file.
 * @return true on success, false on failure and errno is set.
 ***/
bool config_init(ConfigParser *p, const char *config_file_path);

/***
 * Free a configuration parser.
 *
 * @param p The ConfigParser to free.
 ***/
void config_end(ConfigParser *p);

/***
 * Get a string value using 'key' from a configuration file.
 *
 * @param p An initialized ConfigParser.
 * @param key The key to get the value from.
 * @return The value or NULL on failure and errno is set to EINVAL.
 ***/
char *config_get_string(ConfigParser *p, const char *key);

#endif // CONFIG_H
