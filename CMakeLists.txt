# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(Screenclip)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED Widgets)

set(SOURCES
    "main.cpp"
    "screenclip.cpp"
    "screenclip.h"
)

qt5_add_resources(SOURCES screenclip.qrc)

# Add source to this project's executable.
add_executable (Screenclip ${SOURCES})


target_link_libraries(Screenclip Qt5::Widgets)
