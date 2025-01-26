cmake_minimum_required(VERSION 3.16)

set(RELEASE 0)
# add_compile_definitions("DEBUG=$<CONFIG:Debug>")

# set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")# --target=wasm32") # -ftree-parallelize-loops=4 
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

include_directories("${CMAKE_SOURCE_DIR}/mewlib")# "C:/Program Files/LLVM/lib/clang/19/include/openmp_wrappers")

add_subdirectory(raylib)
add_subdirectory(json)

set(CMAKE_TOOLCHAIN_FILE C:/Users/ivans/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)
# add_subdirectory(game_of_life)
# add_subdirectory(craft)
add_subdirectory(tof)
