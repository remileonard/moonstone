cmake_minimum_required(VERSION 3.15)

# Unit tests for libmoon_assets decompressors
add_executable(test_decompressors test_decompressors.c)
target_link_libraries(test_decompressors PRIVATE moon_assets)

add_test(NAME test_decompressors COMMAND test_decompressors)

add_executable(test_cache test_cache.c)
target_link_libraries(test_cache PRIVATE moon_assets)

add_test(NAME test_cache COMMAND test_cache)

# Trace du moteur IMAGEXCEL (utilisée par tools/ix_difftest.py)
add_executable(ix_trace ix_trace.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_vm.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_engine.c)
target_include_directories(ix_trace PRIVATE
    ${CMAKE_SOURCE_DIR}/game/include ${CMAKE_SOURCE_DIR}/game/data)

# Routines portées du combat (utilisée par tools/mog_difftest.py)
add_executable(mog_step mog_step.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_vm.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_engine.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_ctl.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_col.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_native.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_ai.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_setup.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_encounter.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_loop.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_boot.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_gfx.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_blit.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_vbl.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_sound.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_text.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_map.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_screens.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_town.c
    ${CMAKE_SOURCE_DIR}/game/data/ix_mog.c)
target_include_directories(mog_step PRIVATE
    ${CMAKE_SOURCE_DIR}/game/include ${CMAKE_SOURCE_DIR}/game/data)
target_link_libraries(mog_step PRIVATE moon_assets)

# Mémoire de démarrage (utilisée par tools/mog_bootcheck.py)
add_executable(mog_boot_dump mog_boot_dump.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_vm.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_boot.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_gfx.c
    ${CMAKE_SOURCE_DIR}/game/data/ix_mog.c)
target_include_directories(mog_boot_dump PRIVATE
    ${CMAKE_SOURCE_DIR}/game/include ${CMAKE_SOURCE_DIR}/game/data)
target_link_libraries(mog_boot_dump PRIVATE moon_assets)

# Combat complet sans écran, images PNG (game/src/mog_fight.c)
add_executable(mog_fight_shot mog_fight_shot.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_vm.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_engine.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_ctl.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_col.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_native.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_ai.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_setup.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_encounter.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_loop.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_boot.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_gfx.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_fight.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_blit.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_vbl.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_sound.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_text.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_map.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_screens.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_town.c
    ${CMAKE_SOURCE_DIR}/game/data/ix_mog.c)
target_include_directories(mog_fight_shot PRIVATE
    ${CMAKE_SOURCE_DIR}/game/include ${CMAKE_SOURCE_DIR}/game/data)
target_link_libraries(mog_fight_shot PRIVATE moon_assets)

# Programme de mog mené pas à pas (tools/mog_lockstep.py)
add_executable(mog_run mog_run.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_vm.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_engine.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_ctl.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_col.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_native.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_ai.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_setup.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_encounter.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_loop.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_boot.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_gfx.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_blit.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_vbl.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_sound.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_text.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_map.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_screens.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_town.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_game.c
    ${CMAKE_SOURCE_DIR}/game/data/ix_mog.c)
target_include_directories(mog_run PRIVATE
    ${CMAKE_SOURCE_DIR}/game/include ${CMAKE_SOURCE_DIR}/game/data)
target_link_libraries(mog_run PRIVATE moon_assets)
target_compile_definitions(mog_run PRIVATE MOG_VBL_TRACE)

# Jeu complet sans écran, mené par un script (game/src/mog_game.c)
add_executable(mog_game_shot mog_game_shot.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_vm.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_engine.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_ctl.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_col.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_native.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_ai.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_setup.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_encounter.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_loop.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_boot.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_gfx.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_blit.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_vbl.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_sound.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_text.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_map.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_screens.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_town.c
    ${CMAKE_SOURCE_DIR}/game/src/mog_game.c
    ${CMAKE_SOURCE_DIR}/game/data/ix_mog.c)
target_include_directories(mog_game_shot PRIVATE
    ${CMAKE_SOURCE_DIR}/game/include ${CMAKE_SOURCE_DIR}/game/data)
target_link_libraries(mog_game_shot PRIVATE moon_assets)

# Scènes de l'intro de program en C (tools/prog_lockstep.py)
add_executable(prog_run prog_run.c
    ${CMAKE_SOURCE_DIR}/game/src/ix_vm.c
    ${CMAKE_SOURCE_DIR}/game/src/prog_intro.c
    ${CMAKE_SOURCE_DIR}/game/src/prog_blit.c
    ${CMAKE_SOURCE_DIR}/game/src/prog_gfx.c
    ${CMAKE_SOURCE_DIR}/game/src/prog_vbl.c)
target_include_directories(prog_run PRIVATE
    ${CMAKE_SOURCE_DIR}/game/include ${CMAKE_SOURCE_DIR}/game/data)
