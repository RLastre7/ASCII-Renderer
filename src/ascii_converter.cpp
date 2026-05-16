#include "ascii_converter.hpp"
#include <opencv2/imgproc.hpp>

#include <iostream>

struct Pixel {
    Uint8 r, g, b, a;
    Pixel() : r(0), g(0), b(0), a(0) {}
    Pixel(const SDL_Surface* surface, int x, int y);
    void set_rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xFF);
private:
    const SDL_Surface* surface;
    Uint8* pixel;
    int x, y;
    void read_pixel();
    void write_pixel();
};

Pixel::Pixel(const SDL_Surface* surface, int x, int y)
    : surface(surface), x(x), y(y) {
    read_pixel();
    Uint32 pixel_val = 0;
    memcpy(&pixel_val, pixel, surface->format->BytesPerPixel);
    SDL_GetRGB(pixel_val, surface->format, &r, &g, &b);
}

void Pixel::read_pixel() {
    Uint8* row = (Uint8*)surface->pixels + y * surface->pitch;
    pixel = row + x * surface->format->BytesPerPixel;
}

void Pixel::write_pixel() {
    Uint32 mapped = SDL_MapRGB(surface->format, r, g, b);
    memcpy(pixel, &mapped, surface->format->BytesPerPixel);
}

void Pixel::set_rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    this->r = r; this->g = g; this->b = b; this->a = a;
    write_pixel();
}

static SDL_Color get_avg_color_in_range(const SDL_Surface* surface, int x, int y, int range) {
    int sum_r = 0, sum_g = 0, sum_b = 0;
    int count = 0;
    for (int i = x - range; i < x + range; i++) {
        if (i < 0 || i >= surface->w) continue;
        for (int j = y - range; j < y + range; j++) {
            if (j < 0 || j >= surface->h) continue;
            Pixel p(surface, i, j);
            sum_r += p.r;
            sum_g += p.g;
            sum_b += p.b;
            count++;
        }
    }
    Uint8 avg_r = sum_r / count;
    Uint8 avg_g = sum_g / count;
    Uint8 avg_b = sum_b / count;
    return {avg_r , avg_g , avg_b};
}

static SDL_Color get_avg_in_range(const SDL_Surface* surface, int x, int y, int range) {
    int sum = 0;
    int count = 0;
    for (int i = x - range; i < x + range; i++) {
        if (i < 0 || i >= surface->w) continue;
        for (int j = y - range; j < y + range; j++) {
            if (j < 0 || j >= surface->h) continue;
            Pixel p(surface, i, j);
            sum += p.r;
            count++;
        }
    }
    Uint8 avg = sum/count;
    return {avg,avg,avg};
}

static SDL_Surface* mat_to_surface(const cv::Mat& mat) {
    cv::Mat rgb;
    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    SDL_Surface* surf = SDL_CreateRGBSurface(0, rgb.cols, rgb.rows, 24,
                                             0xFF, 0xFF00, 0xFF0000, 0);
    SDL_LockSurface(surf);
    for (int y = 0; y < rgb.rows; y++) {
        memcpy((Uint8*)surf->pixels + y * surf->pitch, rgb.ptr(y), rgb.cols * 3);
    }
    SDL_UnlockSurface(surf);
    return surf;
}

static int get_color_avg(const SDL_Color& color){
    return (color.r + color.g + color.b)/3;
}

ASCIIConverter::ASCIIConverter(const char* font_path, int font_size, SDL_Color text_color,
                               const char** gradient, int gradient_len, bool invert,
                               bool color_mode, int padding)
    : font_size(font_size), text_color(text_color),
      gradient(gradient), gradient_len(gradient_len), surface(nullptr), invert(invert),
      color_mode(color_mode), padding(padding) {
    font = TTF_OpenFont(font_path, font_size);
}

ASCIIConverter::~ASCIIConverter() {
    if (surface) SDL_FreeSurface(surface);
    if (font) TTF_CloseFont(font);
}

void ASCIIConverter::compute_luminance_range(SDL_Surface* img, int& min_val, int& max_val) {
    SDL_LockSurface(img);
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            Pixel p{img, x, y};
            Uint8 lum = 0.2126 * p.r + 0.7152 * p.g + 0.114 * p.b;
            if (lum < min_val) min_val = lum;
            if (lum > max_val) max_val = lum;
        }
    }
    SDL_UnlockSurface(img);
}

void ASCIIConverter::grey_scale(SDL_Surface* img) {
    SDL_LockSurface(img);
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            Pixel p{img, x, y};
            Uint8 avg = 0.2126 * p.r + 0.7152 * p.g + 0.114 * p.b;
            p.set_rgba(avg, avg, avg);
        }
    }
    SDL_UnlockSurface(img);
}

void ASCIIConverter::convert(SDL_Surface* img) {
    int min_val = 255, max_val = 0;

    compute_luminance_range(img, min_val, max_val);
    if(!color_mode) grey_scale(img);

    int val_range = std::max(max_val - min_val,1);

    int char_w, char_h;
    TTF_SizeText(font, "@", &char_w, &char_h);
    if (char_w <= 0) char_w = font_size;
    if (char_h <= 0) char_h = font_size;

    int step = font_size;
    int cols = img->w / step;
    int rows = img->h / step;

    SDL_Surface* result = SDL_CreateRGBSurface(0, cols * char_w + 2 * padding, rows * char_h + 2 * padding, 32, 0, 0, 0, 0);

    for (int y = 0; y + step <= img->h; y += step) {
        for (int x = 0; x + step <= img->w; x += step) {
            
            //get color 
            SDL_Color c = text_color;
            if (color_mode) c = get_avg_color_in_range(img,x,y,font_size);
            else c = get_avg_in_range(img, x, y, font_size);
            
            //determine index
            int avg = get_color_avg(c);
            int idx = ((avg - min_val) * (gradient_len - 1)) / val_range;
            idx = invert ? std::max( (gradient_len - 1) - idx,0 ) : std::min(idx,gradient_len - 1);

            //blit text onto surface
            SDL_Surface* text = TTF_RenderText_Blended(font, gradient[idx], c);
            SDL_Rect dst = {(x / step) * char_w + padding, (y / step) * char_h + padding, text->w, text->h};
            SDL_BlitSurface(text, NULL, result, &dst);
            SDL_FreeSurface(text);
        }
    }

    if (surface) SDL_FreeSurface(surface);
    surface = result;
}

void ASCIIConverter::convert(const cv::Mat& mat) {
    SDL_Surface* img = mat_to_surface(mat);
    convert(img);
    SDL_FreeSurface(img);
}
