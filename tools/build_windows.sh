#!/bin/sh
# Compilation croisée Windows 64 bits (MinGW-w64) de moonstone.exe.
#   tools/build_windows.sh <SDL2_include> <dossier_des_DLL> [sortie]
# <SDL2_include> : dossier contenant SDL2/SDL.h et SDL2/SDL_mixer.h ;
# <dossier_des_DLL> : SDL2.dll et SDL2_mixer.dll (versions officielles).
set -e
INC=$1; DLL=$2; OUT=${3:-moonstone.exe}
cd "$(dirname "$0")/.."
x86_64-w64-mingw32-gcc -O2 -std=c11 -DHAVE_SDL2=1 -DHAVE_SDL2_MIXER=1 \
    -Igame/include -Igame/data -Ilibmoon_assets/include -Ilibmoon_assets/src \
    -I"$INC" -I"$INC/SDL2" \
    game/src/*.c game/data/ix_program.c game/data/ix_mog.c libmoon_assets/src/*.c \
    "$DLL/SDL2.dll" "$DLL/SDL2_mixer.dll" -lm -o "$OUT"
