cmake_minimum_required(VERSION 3.20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 23)

project(unixcmd VERSION 1.0 LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup(KEEP_RPATHS)

find_package(
  Qt6
    COMPONENTS
      Core
      Gui
      Qml
      Quick
      QuickControls2
      Widgets
    REQUIRED
)

qt_standard_project_setup()

add_subdirectory(src)