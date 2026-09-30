cmake_minimum_required(VERSION 3.15)

# Outils en ligne de commande (fichiers lus par libmoon_assets)
add_executable(moon-info moon_info.c)
target_link_libraries(moon-info PRIVATE moon_assets)

add_executable(moon-dump moon_dump.c)
target_link_libraries(moon-dump PRIVATE moon_assets)

add_executable(moon-view moon_view.c)
target_link_libraries(moon-view PRIVATE moon_assets SDL2::SDL2)
target_include_directories(moon-view PRIVATE ${SDL2_INCLUDE_DIRS})

install(TARGETS moon-info moon-dump moon-view RUNTIME DESTINATION bin)
