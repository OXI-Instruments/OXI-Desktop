cmake_minimum_required(VERSION 3.14)

project(oxi_core LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(oxi_core SHARED
  oxi_core_global.h
  midiworker.cpp
  midiworker.h
  OXI_SYSEX_MSG.h
  MIDI.h
  Nibble.h
  crc32.h
  SYSEX_APP.h
  SYSEX_PROJ.h
  delay.h
)

target_include_directories(oxi_core PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(oxi_core
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Widgets
            QMidi
        )
target_compile_definitions(oxi_core
    PRIVATE OXI_CORE_LIBRARY)
