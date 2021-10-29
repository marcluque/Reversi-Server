cmake_minimum_required(VERSION 3.16)
project(reversi_server C)

set(CMAKE_C_STANDARD 11)

include_directories(include/uthash)

add_executable(reversi_server
        src/main.c
        src/network/server.c
        src/network/server.h
        src/map/map.c
        src/map/map.h
        src/map/map_util.h
        src/globals.h
        include/uthash/uthash.h
        src/map/map_loader.c
        src/map/map_loader.h
        src/map/map_util.c)

target_link_libraries(reversi_server -static-libgcc)