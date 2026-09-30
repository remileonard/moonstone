cmake_minimum_required(VERSION 3.15)

# ---------------------------------------------------------------
# Moonstone — game executable
# ---------------------------------------------------------------

set(GAME_SOURCES
    src/main.c
    src/moon_hal.c
    src/moon_game.c
    src/moon_combat.c
    src/ix_vm.c
    src/ix_engine.c
    src/mog_ctl.c
    src/mog_col.c
    src/mog_native.c
    src/mog_ai.c
    src/mog_setup.c
    src/mog_encounter.c
    src/mog_fight.c
    src/mog_blit.c
    src/mog_gfx.c
    src/mog_vbl.c
    src/mog_text.c
    src/mog_map.c
    src/mog_screens.c
    src/mog_town.c
    src/mog_game.c
    src/mog_menu.c
    src/mog_sound.c
    src/moon_mog.c
    src/mog_boot.c
    src/mog_files.c
    src/prog_files.c
    src/mog_loop.c
    src/prog_blit.c
    src/prog_gfx.c
    src/prog_vbl.c
    src/prog_intro.c
    src/prog_music.c
    data/ix_program.c
    data/ix_mog.c
)

add_executable(moonstone ${GAME_SOURCES})

target_include_directories(moonstone
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/data
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

if(EMSCRIPTEN)
    # Un seul moonstone.html : wasm intégré (SINGLE_FILE), données du jeu
    # intégrées sous /data si MOON_WEB_DATA est donné ; ASYNCIFY rend la
    # main au navigateur à chaque attente d'image (hal_vbl_wait).
    set(MOON_WEB_DATA "" CACHE PATH "Dossier des fichiers du jeu à intégrer (usage personnel)")
    set_target_properties(moonstone PROPERTIES SUFFIX ".html")
    target_link_options(moonstone PRIVATE
        -sASYNCIFY -sALLOW_MEMORY_GROWTH -sSINGLE_FILE -sEXIT_RUNTIME=0
        "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html")
    set_property(TARGET moonstone APPEND PROPERTY LINK_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html)
    if(MOON_WEB_DATA)
        target_link_options(moonstone PRIVATE "--embed-file=${MOON_WEB_DATA}@/data")
    else()
        message(WARNING "MOON_WEB_DATA non donné : moonstone.html sans données du jeu")
    endif()
else()
    install(TARGETS moonstone RUNTIME DESTINATION bin)
endif()
