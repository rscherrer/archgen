# CMakeLists.txt (Release)

# CMake
cmake_minimum_required(VERSION 3.16)

# Project name
project(archgen)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boilerplate
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

# Release mode
set(CMAKE_BUILD_TYPE "Release")

# Source code
add_subdirectory(src)