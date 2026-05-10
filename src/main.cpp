#include <opencv2/opencv.hpp>
#include "sdl_context.hpp"
#include "ascii_converter.hpp"

int main() {
    SDLContext ctx = sdl_init(1, 1, "Ascii Renderer");

    int font_size = 3;
    SDL_Color text_color = {255, 255, 255, 255};
    const char* gradient[] = {".", ",", ":", ";", "!", "-", "=", "~", "+", "*", "?", "#", "%", "&", "8", "@", "$", "█"};
    int gradient_len = 18;
    cv::VideoCapture cap("assets/video.mp4");
    if (!cap.isOpened()) {
        printf("error: could not open video\n");
        return 1;
    }

    bool window_resized = false;
    bool open = true;
    cv::Mat frame;
    double prev = 0, currnent = 0;
    {
        ASCIIConverter ascii("assets/DejaVuSans.ttf",
                             font_size, text_color, gradient, gradient_len, true, false);
        // TTF_Font* fps_font = TTF_OpenFont("assets/DejaVuSans.ttf", font_size*2);
        while (open) {
            // prev = currnent;
            // currnent = std::chrono::duration_cast<std::chrono::seconds>(
                        //   std::chrono::system_clock::now().time_since_epoch())
                        //   .count();
            // std::string fps_text = "FPS: " + std::to_string( (currnent-prev) );
            // SDL_Surface* fps_surface = TTF_RenderText_Blended(fps_font, fps_text.c_str(), {255,0,255,255});
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
            // SDL_BlitSurface(fps_surface,NULL,ascii.surface,NULL);
            SDL_BlitScaled(ascii.surface, NULL, ctx.window_surface, &dst);
            SDL_UpdateWindowSurface(ctx.window);
        }
    }

    sdl_cleanup(ctx);
    return 0;
}
