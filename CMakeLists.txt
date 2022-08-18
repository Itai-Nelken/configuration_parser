cmake_minimum_required(VERSION 3.16)

project(libconfig LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_compile_options(-Wall -Wextra)


set(CONFIG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/array.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/token.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scanner.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.c
)

add_library(config ${CONFIG_SOURCES})

add_executable(conf src/main.c)
target_link_libraries(conf PRIVATE config)
