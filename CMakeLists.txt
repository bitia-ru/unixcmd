cmake_minimum_required(VERSION 3.20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 23)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(MACOS TRUE)
endif()

project(unixcmd VERSION 1.0 LANGUAGES CXX)

set(CONAN_CACHE_FOLDER "" CACHE STRING "Conan cache folder")

if(CONAN_CACHE_FOLDER STREQUAL "")
  message(FATAL_ERROR "CONAN_CACHE_FOLDER is not set")
endif()

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
