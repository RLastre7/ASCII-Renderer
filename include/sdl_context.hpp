#pragma once
#include <SDL2/SDL.h>

struct SDLContext {
    SDL_Window* window;
    SDL_Surface* window_surface;
};

SDLContext sdl_init(int width, int height, const char* title);
void sdl_cleanup(SDLContext& ctx);
