cmake_minimum_required(VERSION 3.14)

project(Oxi-Desktop-App LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENV{GOOGLETEST_DIR} "${CMAKE_SOURCE_DIR}/libs/googletest/")

option(BUILD_TESTS "Build unit tests." ON)

add_subdirectory(libs/rtmidi)
add_subdirectory(libs/QMidi)
add_subdirectory(libs/oxi/oxi_core)
add_subdirectory(libs/midifile)
add_subdirectory(apps/oxi_desktop)
add_subdirectory(tests/unittests)
