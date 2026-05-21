cmake_minimum_required(VERSION 3.15)

# ---------------------------------------------------------------
# Moonstone — game executable
# ---------------------------------------------------------------

set(GAME_SOURCES
    src/main.c
    src/moon_hal.c
    src/moon_render.c
    src/moon_entity.c
    src/moon_game.c
    src/moon_intro.c
    src/moon_menu.c
    src/moon_overworld.c
    src/moon_combat.c
    src/moon_town.c
    src/moon_shop.c
    src/moon_wizard.c
    src/moon_village.c
    src/moon_stonehenge.c
    src/moon_valley.c
    src/moon_ending.c
)

add_executable(moonstone ${GAME_SOURCES})

target_include_directories(moonstone
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libmoon_assets/include
        ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(moonstone
    PRIVATE
        moon_assets
        SDL2::SDL2
        m
)

target_compile_definitions(moonstone PRIVATE HAVE_SDL2=1)

if(SDL2_mixer_FOUND)
    target_link_libraries(moonstone PRIVATE SDL2_mixer::SDL2_mixer)
    target_compile_definitions(moonstone PRIVATE HAVE_SDL2_MIXER=1)
endif()

install(TARGETS moonstone RUNTIME DESTINATION bin)
