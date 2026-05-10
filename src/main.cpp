#include <opencv2/opencv.hpp>
#include "sdl_context.hpp"
#include "ascii_converter.hpp"

int main() {
    SDLContext ctx = sdl_init(1, 1, "Title");

    int font_size = 5;
    SDL_Color text_color = {255, 255, 255, 255};
    const char* gradient[] = {".", ":", "-", "=", "+", "*", "#", "%", "@", "█"};
    int gradient_len = 10;

    cv::VideoCapture cap("assets/video.mp4");
    if (!cap.isOpened()) {
        printf("error: could not open video\n");
        return 1;
    }

    bool window_resized = false;
    bool open = true;
    cv::Mat frame;

    {
        ASCIIConverter ascii("assets/DejaVuSans.ttf",
                             font_size, text_color, gradient, gradient_len);

        while (open) {
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
            SDL_BlitScaled(ascii.surface, NULL, ctx.window_surface, &dst);
            SDL_UpdateWindowSurface(ctx.window);
        }
    }

    sdl_cleanup(ctx);
    return 0;
}
