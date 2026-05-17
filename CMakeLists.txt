cmake_minimum_required(VERSION 3.15)
project(moonstone VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------
# libmoon_assets
# ---------------------------------------------------------------
add_library(moon_assets STATIC
    libmoon_assets/src/moon_assets.c
    libmoon_assets/src/rnc1.c
    libmoon_assets/src/lzss_cel.c
    libmoon_assets/src/rle_stile.c
    libmoon_assets/src/packbits_piv.c
)

target_include_directories(moon_assets
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/libmoon_assets/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libmoon_assets/src
)

# ---------------------------------------------------------------
# Optional SDL2 for tools
# ---------------------------------------------------------------
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SDL2 sdl2)
endif()

# ---------------------------------------------------------------
# CLI tools
# ---------------------------------------------------------------
add_subdirectory(tools)

# ---------------------------------------------------------------
# Tests
# ---------------------------------------------------------------
enable_testing()
add_subdirectory(tests)
