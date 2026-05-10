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

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build . --target app
```

## Run

```bash
./build/app
```

Place a video file at `assets/video.mp4` or edit the path in `src/main.cpp`.

## Dependencies

- SDL2, SDL2_image, SDL2_ttf
- OpenCV 4
