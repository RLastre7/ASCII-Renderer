#include "ascii_converter.hpp"
#include <opencv2/imgproc.hpp>

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

static void get_avg_color_in_range(const SDL_Surface* surface, int x, int y, int range,
                                    int& avg_r, int& avg_g, int& avg_b, int& avg_lum) {
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
    avg_r = sum_r / count;
    avg_g = sum_g / count;
    avg_b = sum_b / count;
    avg_lum = 0.2126 * avg_r + 0.7152 * avg_g + 0.114 * avg_b;
}

static int get_avg_in_range(const SDL_Surface* surface, int x, int y, int range) {
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
    return sum / count;
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

ASCIIConverter::ASCIIConverter(const char* font_path, int font_size, SDL_Color text_color,
                               const char** gradient, int gradient_len, bool invert,
                               bool color_mode)
    : font_size(font_size), text_color(text_color),
      gradient(gradient), gradient_len(gradient_len), surface(nullptr), invert(invert),
      color_mode(color_mode) {
    font = TTF_OpenFont(font_path, font_size);
}

ASCIIConverter::~ASCIIConverter() {
    if (surface) SDL_FreeSurface(surface);
    if (font) TTF_CloseFont(font);
}

void ASCIIConverter::convert(SDL_Surface* img) {
    int min_val = 255, max_val = 0;

    SDL_LockSurface(img);
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            Pixel p{img, x, y};
            Uint8 avg = 0.2126 * p.r + 0.7152 * p.g + 0.114 * p.b;
            if (!color_mode) p.set_rgba(avg, avg, avg);
            if (avg < min_val) min_val = avg;
            if (avg > max_val) max_val = avg;
        }
    }
    SDL_UnlockSurface(img);

    int val_range = max_val - min_val;
    if (val_range == 0) val_range = 1;

    int char_w, char_h;
    TTF_SizeText(font, "@", &char_w, &char_h);
    if (char_w <= 0) char_w = font_size;
    if (char_h <= 0) char_h = font_size;

    int step = font_size;
    int cols = img->w / step;
    int rows = img->h / step;

    SDL_Surface* result = SDL_CreateRGBSurface(0, cols * char_w, rows * char_h, 32, 0, 0, 0, 0);

    for (int y = 0; y + step <= img->h; y += step) {
        for (int x = 0; x + step <= img->w; x += step) {
            int avg;
            SDL_Color c = text_color;

            if (color_mode) {
                int avg_r, avg_g, avg_b, avg_lum;
                get_avg_color_in_range(img, x, y, font_size, avg_r, avg_g, avg_b, avg_lum);
                avg = avg_lum;
                c = {(Uint8)avg_r, (Uint8)avg_g, (Uint8)avg_b, 255};
            } else {
                avg = get_avg_in_range(img, x, y, font_size);
            }

            int idx = ((avg - min_val) * (gradient_len - 1)) / val_range;
            idx = invert ? (gradient_len - 1) - idx : idx;
            if (idx < 0) idx = 0;
            if (idx >= gradient_len) idx = gradient_len - 1;

            SDL_Surface* text = TTF_RenderText_Blended(font, gradient[idx], c);
            SDL_Rect dst = {(x / step) * char_w, (y / step) * char_h, text->w, text->h};
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
