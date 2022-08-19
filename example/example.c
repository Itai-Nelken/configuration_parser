#include <stdio.h>
#include <config_parser.h>

int main(void) {
    ConfigParser p;
    ConfigTable *root = config_parse(&p, "./test.config");
    if(!root) {
        puts("Failed to parse config file!");
        return 1;
    }

    ConfigValue val = config_get_string(root, "a");
    if(val.ok) {
        printf("a = %s\n", val.as.string);
    } else {
        puts("Error getting 'a' key!");
    }

    ConfigTable *test = config_get_table(&p, "test");
    if(!test) {
        puts("Error getting 'test' table");
        goto end;
    }

    val = config_get_boolean(test, "valid");
    if(val.ok) {
        printf("test::valid = %s\n", val.as.boolean ? "true" : "false");
    } else {
        puts("Error getting 'valid' key in table 'test'");
    }

    ConfigTable *test2 = config_get_table(&p, "test2");
    if(!test2) {
        puts("Error getting 'test2' table");
        goto end;
    }

    val = config_get_boolean(test2, "valid");
    if(val.ok) {
        printf("test2::valid = %s\n", val.as.boolean ? "true" : "false");
    } else {
        puts("Error getting 'valid' key in table 'test2'");
    }

end:
    config_end(&p);
    return 0;
}
