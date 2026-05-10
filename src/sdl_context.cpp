#include "sdl_context.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

SDLContext sdl_init(int width, int height, const char* title) {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow(title, 500, 0, width, height, 0);
    return {window, SDL_GetWindowSurface(window)};
}

void sdl_cleanup(SDLContext& ctx) {
    SDL_DestroyWindow(ctx.window);
    TTF_Quit();
    SDL_Quit();
}
