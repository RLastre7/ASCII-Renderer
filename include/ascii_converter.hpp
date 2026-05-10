#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <opencv2/core.hpp>

struct ASCIIConverter {
    TTF_Font* font;
    int font_size;
    SDL_Color text_color;
    const char** gradient;
    int gradient_len;
    SDL_Surface* surface;
    bool invert;

    ASCIIConverter(const char* font_path, int font_size, SDL_Color text_color,
                   const char** gradient, int gradient_len,bool invert = false);
    ~ASCIIConverter();

    void convert(SDL_Surface* img);
    void convert(const cv::Mat& mat);
};
