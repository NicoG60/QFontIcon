cmake_minimum_required(VERSION 3.14)

project(QFontIcon VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(QFONTICON_BUILD_EXAMPLE "Build the nice example" OFF)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_library(QFontIcon
  src/qfonticon.cpp
  include/qfonticon.h
)

target_link_libraries(QFontIcon PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
target_include_directories(QFontIcon PUBLIC include)

if(QFONTICON_BUILD_EXAMPLE)
    add_executable(example example/main.cpp example/fonts.qrc)
    target_link_libraries(example PUBLIC QFontIcon)
endif()
