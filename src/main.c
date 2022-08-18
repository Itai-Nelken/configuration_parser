#include <stdio.h>
#include "config.h"

int main(void) {
    ConfigParser p;
    if(!config_init(&p, "./test.config")) {
        perror("config_init()");
        return 1;
    }

    char *value = NULL;
    if((value = config_get_string(&p, "test"))) {
        printf("test = '%s'\n", value);
    } else {
        puts("Error getting 'test' key!");
    }

    config_end(&p);
    return 0;
}
