# ASCII Art Video Converter

Real-time ASCII art renderer for video files using SDL2 and OpenCV.

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
    ├── gojo.jpg
    └── video.mp4
```

## Dependencies

You can either install system packages or use vcpkg.

### Option 1: System packages

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

### Option 2: vcpkg (cross-platform, no system packages needed)

```bash
# Install vcpkg (one-time)
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
export VCPKG_ROOT="$PWD/vcpkg"

# Build
cmake --preset vcpkg
cmake --build build
```

## Build

**With system packages:**
```bash
cmake -B build
cmake --build build
```

**With vcpkg:**
```bash
cmake --preset vcpkg
cmake --build build
```

## Run

```bash
./build/app
```

Place a video file at `assets/video.mp4` or edit the path in `src/main.cpp`.

To use your webcam instead, change `cv::VideoCapture cap("assets/video.mp4")` to `cv::VideoCapture cap(0)` in `src/main.cpp`.
