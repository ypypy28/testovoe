cmake_minimum_required(VERSION 3.20)

project(task0)

# build out of source in the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)


if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(${PROJECT_NAME} textsdl2.c)
target_link_libraries(${PROJECT_NAME}  ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})