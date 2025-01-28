cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(botman
    VERSION 1.0
    LANGUAGES CXX C
)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG c1ab645ca298a2801097931d1079b10ff7eb9df8
)

FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG 25c8c65a6e5f0f4d4b564a0343861898c6f2778b
)

FetchContent_MakeAvailable(raylib raygui)

add_executable(botman
    src/main.cpp
    src/constants.hpp
    src/map.cpp src/map.hpp
    src/robot.cpp src/robot.hpp
)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0 -Wall -Werror -fsanitize=address -fsanitize=leak")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

target_link_libraries(botman PUBLIC raylib)
target_include_directories(botman PRIVATE ${FETCHCONTENT_BASE_DIR}/raygui-src/src)
target_compile_definitions(botman PRIVATE ROOT_PATH="${CMAKE_SOURCE_DIR}")
