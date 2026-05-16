#include <opencv2/opencv.hpp>
#include "sdl_context.hpp"
#include "ascii_converter.hpp"
#include <chrono>

std::string get_fps(std::chrono::steady_clock::time_point& prev,std::chrono::steady_clock::time_point& current,int& frames_elapsed,int& fps) {
    current = std::chrono::steady_clock::now();
    frames_elapsed++;
    if( (current - prev) >= std::chrono::seconds(1) ) {
        fps = frames_elapsed;
        frames_elapsed = 0;
        prev = current;
    }
    return "FPS: " + std::to_string(fps);
}

int main() {
    SDLContext ctx = sdl_init(1, 1, "Ascii Renderer");

    int font_size = 20;
    SDL_Color text_color = {255, 255, 255, 255};
    const char* gradient[] = {".", ",", ":", ";", "!", "-", "=", "~", "+", "*", "?", "#", "%", "&", "8", "@", "$", "█"};
    int gradient_len = 18;
    cv::VideoCapture cap("assets/video-2.mp4");
    if (!cap.isOpened()) {
        printf("error: could not open video\n");
        return 1;
    }

    bool window_resized = false;
    bool open = true;
    cv::Mat frame;
    auto prev = std::chrono::steady_clock::now();
    auto current = std::chrono::steady_clock::now();
    int fps = 0;
    int frames_elapsed = 0;
    std::string fps_text = "FPS: 0";
    {
        int padding = font_size * 10;
        ASCIIConverter ascii("assets/DejaVuSans.ttf", font_size, text_color, gradient, gradient_len, true, true, padding);
        TTF_Font* fps_font = TTF_OpenFont("assets/DejaVuSans.ttf", font_size*10);
        while (open) {
            fps_text = get_fps(prev,current,frames_elapsed,fps);
            SDL_Surface* fps_surface = TTF_RenderText_Blended(fps_font, fps_text.c_str(), {255,255,255,255});

            SDL_ShowWindow(ctx.window);
            SDL_Event event;
            while (SDL_PollEvent(&event))
                if (event.type == SDL_QUIT) open = false;

            cap.read(frame);
            if (frame.empty()) {
                cap.set(cv::CAP_PROP_POS_FRAMES, 0);
                continue;
            }

            ascii.convert(frame);

            if (!window_resized) {
                SDL_SetWindowSize(ctx.window, ascii.surface->w, ascii.surface->h);
                window_resized = true;
            }

            ctx.window_surface = SDL_GetWindowSurface(ctx.window);
            int win_w, win_h;
            SDL_GetWindowSize(ctx.window, &win_w, &win_h);
            SDL_Rect dst = {0, 0, win_w, win_h};
            SDL_BlitSurface(fps_surface,NULL,ascii.surface,NULL);
            SDL_BlitScaled(ascii.surface, NULL, ctx.window_surface, &dst);
            SDL_UpdateWindowSurface(ctx.window);
            SDL_FreeSurface(fps_surface);
        }
    }

    sdl_cleanup(ctx);
    return 0;
}
