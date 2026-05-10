# ASCII Art Video Converter

Real-time ASCII art renderer for video files using SDL2 and OpenCV.

## Getting Started

```bash
git clone https://github.com/RLastre7/Ascii-Renderer.git
cd Ascii-Renderer
```

Then follow the build instructions below.

## Structure

```
├── CMakeLists.txt
├── README.md
├── include/          # public headers
│   ├── ascii_converter.hpp
│   └── sdl_context.hpp
├── src/              # source files
│   ├── main.cpp
│   ├── ascii_converter.cpp
│   └── sdl_context.cpp
└── assets/           # sample media
    ├── DejaVuSans.ttf
    └── video.mp4
```

## Dependencies

### System packages

**Ubuntu/Debian:**
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libopencv-dev
```

**Fedora:**
```bash
sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel opencv-devel
```

**macOS (Homebrew):**
```bash
brew install sdl2 sdl2_image sdl2_ttf opencv
```

## Build

**Linux/macOS (system packages):**
```bash
cd Ascii-Renderer
cmake -B build
cmake --build build
```

## Run

**Linux/macOS:**
```bash
./build/app
```

Place a video file at `assets/video.mp4` or edit the path in `src/main.cpp`.

To use your webcam instead, change `cv::VideoCapture cap("assets/video.mp4")` to `cv::VideoCapture cap(0)` in `src/main.cpp`.
