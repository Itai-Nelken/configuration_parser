# configuration_parser
A configuration parser C library that parses a subset of TOML.

## Installation
To install the library as a shared object, clone this repository and run the foolowing from its root:
```shell
mkdir build
cd build
cmake ..
make
sudo make install
```
Note that root privileges are needed to install.
You can change the install prefix by passing `-DCMAKE_INSTALL_PREFIX=/path/to/somwhere` to cmake before the `..`.<br>

## Example
See the [example](./example/) folder in this repository.

## Usage
Assuming the following configuration file:
```toml
is_day = true

[Joe]
age = 42
favorite_food = "Vegetable soup"
```
The following code will read it:
```c
#include <stdio.h>
#include <stdlib.h>
#include "config.h"

int main(void) {
  ConfigParser p;
  ConfigTable *conf = config_parse(&p, "./example.config");
  if(!conf) {
    // Parsing failed.
    return EXIT_FAILURE;
  }
  
  // Reading top-level pairs:
  ConfigValue val = config_get_string(conf, "is_day");
  if(val.ok) {
    printf("it is %s\n", val.as.boolean ? "day" : "night");
  }
  
  // Getting a table:
  ConfigTable *joe = config_get_table(&p, "Joe");
  if(!joe) {
    return EXIT_FAILURE;
  }
  
  // Reading the pairs in the table:
  int age = 0;
  val = config_get_number(joe, "age");
  if(val.ok) {
    age = val.as.number;
  }

  val = config_get_string(joe, "favorite_food");
  if(val.ok) {
    printf("Joe is %d years old and his favorite food is %s\n", age, val.as.string);
  }
  
  config_end(&p);
  return EXIT_SUCCESS;
}
```

The state of the parser is stored in the `ConfigParser` struct.
The `config_parse()` function initializes it and stores the parsed configuration file in it.
All the data is owned by the `ConfigParser` struct and is freed when `config_end()` is called.<br>

A configuration file is represented as a table containing all the pairs, the tables are stored in the `ConfigParser` struct and can accessed using `config_get_table()`.<br>

The following functions are used to get a value from a table.<br>
They return a `ConfigValue` that has an `ok` boolean field and a union containing all the possible value types.<br>
If `ok` is true, the value is valid and can be accessed. it is important to use the correct field in the `as` union.
```c
ConfigValue config_get_string(ConfigTable *t, const char *key);
ConfigValue config_get_number(ConfigTable *t, const char *key);
ConfigValue config_get_boolean(ConfigTable *t, const char *key);
```


