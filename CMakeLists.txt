cmake_minimum_required(VERSION 3.16)

set(RELEASE 0)
add_compile_definitions("DEBUG=$<CONFIG:Debug>")

# set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3")
set(CMAKE_CXX_FLAGS "-ftree-parallelize-loops=4")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

include_directories("${CMAKE_SOURCE_DIR}/mewlib")
add_subdirectory(raylib)
add_subdirectory(json)
# include_directories("${raylib_public_headers}")
add_subdirectory(game_of_life)
add_subdirectory(craft)
