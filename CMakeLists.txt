cmake_minimum_required(VERSION 3.16)
project(ascii-art)

list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sdl2_ttf_local/usr")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(app
    src/main.cpp
    src/ascii_converter.cpp
    src/sdl_context.cpp
)

target_include_directories(app PRIVATE
    include
    "${CMAKE_CURRENT_SOURCE_DIR}/sdl2_ttf_local/usr/include"
)

target_link_libraries(app PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    ${OpenCV_LIBS}
)

add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR} $<TARGET_FILE:app>
    DEPENDS app
)
