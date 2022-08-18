cmake_minimum_required(VERSION 3.16)

project(libconfig LANGUAGES C)

set(CONFIG_SOURCES src/config.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_compile_options(-Wall -Wextra)

add_library(config ${CONFIG_SOURCES})

add_executable(conf src/main.c)
target_link_libraries(conf PRIVATE config)
