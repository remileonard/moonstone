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
    libmoon_assets/src/cel.c
    libmoon_assets/src/piv.c
    libmoon_assets/src/stile.c
    libmoon_assets/src/mod.c
    libmoon_assets/src/ob.c
    libmoon_assets/src/hit.c
    libmoon_assets/src/sfx.c
    libmoon_assets/src/terrain.c
    libmoon_assets/src/rnc1.c
    libmoon_assets/src/lzss_cel.c
    libmoon_assets/src/testmap.c
)

target_include_directories(moon_assets
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/libmoon_assets/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libmoon_assets/src
)

# ---------------------------------------------------------------
# Version web (Emscripten) : le jeu seul, en un fichier HTML
#   emcmake cmake -S . -B build-web -DMOON_WEB_DATA=<dossier des données>
# (voir game/web/README.md)
# ---------------------------------------------------------------
if(EMSCRIPTEN)
    add_library(SDL2::SDL2 INTERFACE IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
        INTERFACE_COMPILE_OPTIONS "-sUSE_SDL=2"
        INTERFACE_LINK_OPTIONS "-sUSE_SDL=2")
    add_library(SDL2_mixer::SDL2_mixer INTERFACE IMPORTED)
    set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
        INTERFACE_COMPILE_OPTIONS "-sUSE_SDL_MIXER=2;-sSDL2_MIXER_FORMATS=[]"
        INTERFACE_LINK_OPTIONS "-sUSE_SDL_MIXER=2;-sSDL2_MIXER_FORMATS=[]")
    set(SDL2_mixer_FOUND TRUE)
    add_subdirectory(game)
    return()
endif()

# ---------------------------------------------------------------
# SDL2 and SDL2_mixer (both required) for tools
# ---------------------------------------------------------------
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

# ---------------------------------------------------------------
# CLI tools
# ---------------------------------------------------------------
add_subdirectory(tools)

# ---------------------------------------------------------------
# Game executable
# ---------------------------------------------------------------
add_subdirectory(game)

# ---------------------------------------------------------------
# Tests
# ---------------------------------------------------------------
enable_testing()
add_subdirectory(tests)
