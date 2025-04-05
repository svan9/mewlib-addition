cmake_minimum_required(VERSION 3.16)

set(RELEASE 0)
# add_compile_definitions("DEBUG=$<CONFIG:Debug>")

# set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")# --target=wasm32-wasi  -emit-llvm")# ") # -ftree-parallelize-loops=4 
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

include_directories("${CMAKE_SOURCE_DIR}/mewlib")
# include_directories("${CMAKE_SOURCE_DIR}/coroutines")

add_subdirectory(raylib)
add_subdirectory(json)

set(CMAKE_TOOLCHAIN_FILE C:/Users/ivans/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)

add_subdirectory(game_of_life)
# add_subdirectory(craft)
# add_subdirectory(tof)
add_subdirectory(system_test)
