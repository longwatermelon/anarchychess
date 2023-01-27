#!/bin/sh
em++ -O2 -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sSDL2_IMAGE_FORMATS='["png"]' --preload-file res -std=c++17 src/*.cpp -o docs/index.html
