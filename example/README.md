# Example
This folder contains an example that utilizes all the supported features.

## Compiling
To compile the example, first install the shared library (See the readme at the root of the repository) or compile with `libconfig_static.a` which can be found in the build folder after compiling.
### Compiling and linking with the shared library:
```shell
cc example.c -lconfig -o example
```
### Compiling and linking with the static library
Assuming the build folder is `../build` relative to this folder:
```shell
cc example.c ../build/libconfig_static.a -I../include -o example
```

## Troubleshooting
### problem finding the shared library
If you get an error similar to this one:
```
./example: error while loading shared libraries: libconfig.so: cannot open shared object file: No such file or directory
```
Assuming you used the default prefix of `/usr/local`,
you can either set the environment variable `LD_LIBRARY_PATH` to `/usr/local/lib`
or uninstall the library (remove `/usr/local/lib/libconfig.so` and `/usr/local/include/config_parser.h`) and build it again providing cmake with the following argument before the `..`: `-DCMAKE_INSTALL_PREFIX=/usr`.
If none of that works, you can compile with the static library.
