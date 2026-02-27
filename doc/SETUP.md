## Example setup

In this guide we show how to build the program using the [CMake](https://cmake.org/) build system. Below we provide a ready-made CMake configuration file but the user is free to use any custom configuration.

### Prerequisites

- Any modern C++ compiler (e.g. [GCC](https://gcc.gnu.org/), [Clang](https://clang.llvm.org/) or [MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/))
- Build system: [CMake](https://cmake.org/) (>= 3.16)
- Source retrieval: [git](https://git-scm.com/) (optional)

### Download the code

You can download the source code using `git`:

```shell
git clone https://github.com/rscherrer/archgen
```

Make sure you run the following commands from within the root directory of the repo by using:

```bash
cd archgen
```

### Configure CMake

Our setup uses the following `CMakeLists.txt` configuration file, which you can also find [here](../dev/cmake/release.cmake). Make sure to have it saved in the root directory of the repository.

```cmake
# CMakeLists.txt

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
```

Then, make sure to have this other `CMakeLists.txt` file, this time saved in the `src/` folder. Normally, it should already be present in the right folder if you downloaded this repository.

```cmake
# src/CMakeLists.txt

# Collect source and header files
file(GLOB_RECURSE src 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Instruct CMake to build the binary
add_executable(archgen "${CMAKE_SOURCE_DIR}/main.cpp" ${src})

# Place the binary into ./bin/
set_target_properties(archgen PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<0:>)
```

### Build the program

Then, run the following code from within the repository to create a build folder and instruct CMake to build the program in release mode according to the instructions given in the `CMakeLists.txt` configuration file.

```shell 
cmake -S . -B build
cd build
cmake --build .
```

This setup places the compiled executable in the `bin/` folder.

Note that the steps are **the same on Windows, MacOS and Linux**, although the installation steps of a suitable C++ compiler will typically differ from platform to platform (e.g. environment variables on Windows), and so we refer the reader to online documentation for that.

### IDEs

Many IDEs such as [Visual Studio](https://visualstudio.microsoft.com/) or [XCode](https://developer.apple.com/xcode/) support CMake out of the box. "Open folder" should do the trick...

You can use CMake to generate the input files for your favorite IDE too. For example, running the following from within the recently created `build/` folder will generate Visual Studio project files:

```shell
cmake -G "Visual Studio 17 2022" -A x64 ..
```

For XCode, run:

```shell
cmake -G Xcode    
```

This will place the project files in `build/`.

### Note

During development the script `build.sh` was used to build the program. It is located in the `dev/` folder and should work fine on a Unix-like system. See [here](../dev/README.md) for more details.