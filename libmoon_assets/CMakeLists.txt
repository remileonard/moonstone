cmake_minimum_required(VERSION 3.15)
project(libmoon_assets VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------
# libmoon_assets static library
# ---------------------------------------------------------------
add_library(moon_assets STATIC
    src/moon_assets.c
    src/cel.c
    src/piv.c
    src/stile.c
    src/mod.c
    src/ob.c
    src/hit.c
    src/sfx.c
    src/rnc1.c
    src/lzss_cel.c
    src/rle_stile.c
    src/packbits_piv.c
)

target_include_directories(moon_assets
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# SDL2 is optional for the library itself; tools link it separately
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SDL2 QUIET sdl2)
endif()

# ---------------------------------------------------------------
# CLI tools
# ---------------------------------------------------------------
add_subdirectory(../tools ${CMAKE_BINARY_DIR}/tools)

# ---------------------------------------------------------------
# Tests
# ---------------------------------------------------------------
enable_testing()
add_subdirectory(../tests ${CMAKE_BINARY_DIR}/tests)
