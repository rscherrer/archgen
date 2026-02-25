## Test configuration

In this guide we show how to modify our [main CMake setup](SETUP.md) in order to run the tests that come with the program. Those tests were written with the [Boost.Test](https://www.boost.org/doc/libs/1_86_0/libs/test/doc/html/index.html) C++ library, and here we show how to build them as separate executables that can be used to check the proper behavior of the program. Check out [this page](https://en.wikipedia.org/wiki/Test-driven_development) for more information about **_unit testing_** and **_test-driven development_** (TDD).

### Prerequisites

- Any modern C++ compiler (e.g. [GCC](https://gcc.gnu.org/), [Clang](https://clang.llvm.org/) or [MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/))
- Build system: [CMake](https://cmake.org/) (>= 3.16)
- Source retrieval: [git](https://git-scm.com/)

([Boost.Test](https://www.boost.org/doc/libs/1_86_0/libs/test/doc/html/index.html) and [vcpkg](https://vcpkg.io/) will be downloaded in the process.)

### Download the code 

First, download the repository and set it as working directory if you have not already done so:

```shell
git clone https://github.com/rscherrer/archgen
cd archgen
```

### Configure CMake

Make sure to save the following as a `CMakeLists.txt` file located in the root of the repository. This file is also available [here](../dev/cmake/tests.cmake).

```cmake
# CMakeLists.txt

# CMake
cmake_minimum_required(VERSION 3.16)

# Set up vcpkg
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
if (WIN32)
    set(VCPKG_TARGET_TRIPLET x64-windows)
endif()

# Project name
project(archgen)

# Git will be needed to install Boost
find_package(Git REQUIRED)
execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --remote
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
if (NOT GIT_SUBMOD_RESULT EQUAL "0")
    message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boilerplate
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

# Debug mode
set(CMAKE_BUILD_TYPE "Debug")

# Source code
add_subdirectory(src)
add_subdirectory(tests)
```

This file is substantially longer than the one used in our [main setup](SETUP.md), as it now contains all the build information needed to download the necessary tools from the [Boost](https://www.boost.org/) library for unit tests, and compile those tests using CMake. 

Note that the source code for the tests is located in `tests/`.

On top of that file, make sure to have the following `CMakeLists.txt` file saved in the `src/` directory (it is the same one as in our [main setup](SETUP.md)):

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

Finally, make sure that the following `CMakeLists.txt` file is saved in the `tests/` directory (this file should be already provided in the right folder within this repository):

```cmake
# tests/CMakeLists.txt

# Find Boost
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Model 'unit' files
file(GLOB_RECURSE unit ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Include test utilities
include_directories(${CMAKE_SOURCE_DIR}/tests)

# Automatically find all test files ending with *tests.cpp in the tests directory
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*tests.cpp)

# Build each executable
foreach(TEST_SOURCE IN LISTS TEST_SOURCES)
    # Extract the name of the test executable from the filename
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    # Create the test executable
    add_executable(${TEST_NAME} ${TEST_SOURCE} ${unit} ${CMAKE_SOURCE_DIR}/tests/testutils.cpp)
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/tests)
    target_link_libraries(${TEST_NAME} PUBLIC Boost::unit_test_framework)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests/$<0:>)
endforeach()
```

### Install Boost

Run the following from within the root of the repo to install the testing framework through `git`:

```shell
git submodule add --force https://github.com/microsoft/vcpkg
git submodule update --init --remote
```

These steps will connect the repo to Microsoft's [vcpkg](https://vcpkg.io/) installer, whose job will be to download the required elements from [Boost](https://www.boost.org/) necessary for unit testing (instructions for that are given in the file `vcpkg.json` provided with this repo). 

This setup has the advantage of integrating with CMake and working well across platforms, so the steps are **the same on Windows, Linux or MacOS**. However, in case errors occur during this installation process, we recommend to refer to the log files created by `vcpkg` (some dependencies may be missing and they will be listed there if that is the case).

### Build the tests

Then, run:

```shell
cmake -S . -B build
cd build
cmake --build .
```

Here, the `cmake` command downloads the necessary dependencies through `vcpkg`, and builds all the targets (in debug mode) required by the `CMakeLists.txt` configuration --- not just the program itself, but also the tests, whose executables can be found in `bin/tests/`.

### Run the tests

Simply run those executables to run the tests. For example:

```shell
cd bin/tests
./tests
```

### Note

The `build.sh` script from the `dev/` folder was used during development to build the tests, followed by `run_tests.sh`, still in `dev/`. They should work fine on a Unix-like system. See [here](../dev/README.md) for more details.