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
add_moon_tool(moon-mod-info  moon_mod_info.c)
add_moon_tool(moon-view-test moon_view_test.c)

# moon-anim-dw1 uses two source files (imagexcel engine + test program)
add_executable(moon-anim-dw1 imagexcel.c moon_anim_dw1.c)
target_link_libraries(moon-anim-dw1 PRIVATE moon_assets SDL2::SDL2 m)
target_compile_definitions(moon-anim-dw1 PRIVATE HAVE_SDL2=1)
target_include_directories(moon-anim-dw1 PRIVATE ${SDL2_INCLUDE_DIRS})
if(SDL2_mixer_FOUND)
    target_link_libraries(moon-anim-dw1 PRIVATE SDL2_mixer::SDL2_mixer)
    target_compile_definitions(moon-anim-dw1 PRIVATE HAVE_SDL2_MIXER=1)
endif()

# moon-anim-viewer: generic multi-animation viewer (imagexcel engine + viewer)
add_executable(moon-anim-viewer imagexcel.c moon_anim_viewer.c)
target_link_libraries(moon-anim-viewer PRIVATE moon_assets SDL2::SDL2 m)
target_compile_definitions(moon-anim-viewer PRIVATE HAVE_SDL2=1)
target_include_directories(moon-anim-viewer PRIVATE ${SDL2_INCLUDE_DIRS})
if(SDL2_mixer_FOUND)
    target_link_libraries(moon-anim-viewer PRIVATE SDL2_mixer::SDL2_mixer)
    target_compile_definitions(moon-anim-viewer PRIVATE HAVE_SDL2_MIXER=1)
endif()

install(TARGETS
    moon-info moon-dump moon-palette
    moon-view-cel moon-view-piv moon-view-stile moon-mod-info
    moon-view-test moon-anim-dw1 moon-anim-viewer
    RUNTIME DESTINATION bin
)
