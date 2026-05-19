cmake_minimum_required(VERSION 3.15)

# Helper to add a simple single-file tool
function(add_moon_tool name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE moon_assets SDL2::SDL2 m)
    target_compile_definitions(${name} PRIVATE HAVE_SDL2=1)
    target_include_directories(${name} PRIVATE ${SDL2_INCLUDE_DIRS})
    if(SDL2_mixer_FOUND)
        target_link_libraries(${name} PRIVATE SDL2_mixer::SDL2_mixer)
        target_compile_definitions(${name} PRIVATE HAVE_SDL2_MIXER=1)
    endif()
endfunction()

add_moon_tool(moon-info     moon_info.c)
add_moon_tool(moon-dump     moon_dump.c)
add_moon_tool(moon-palette  moon_palette.c)
add_moon_tool(moon-view-cel moon_view_cel.c)
add_moon_tool(moon-view-piv moon_view_piv.c)
add_moon_tool(moon-view-stile moon_view_stile.c)
add_moon_tool(moon-mod-info moon_mod_info.c)

install(TARGETS
    moon-info moon-dump moon-palette
    moon-view-cel moon-view-piv moon-view-stile moon-mod-info
    RUNTIME DESTINATION bin
)
