cmake_minimum_required(VERSION 3.14)

# This variable is ignored on non Apple devices and needs to be a Cached string
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")

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
add_subdirectory(apps/oxi_desktop)
add_subdirectory(tests/unittests)
