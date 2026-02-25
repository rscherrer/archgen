## Development tools

The specific scripts and files used during the development of the program can be found in the `dev/` folder. 

### Configurations

The `dev/` folder contains a subfolder called `cmake/`, in which we store the CMake configuration files used for various tasks, including:

* `release.cmake` to compile in release mode for the end user (default)
* `tests.cmake` to compile the tests in debug mode and check for memory leaks
* `profile.cmake` to profile the code in release mode 
* `coverage.cmake` to analyze code coverage across tests

To use any of these build configurations, simply copy their content in the `CMakeLists.txt` file located in the root folder, and build the project with CMake (as shown [here](../doc/SETUP.md) and [here](../doc/TESTS.md)). The `CMakesLists.txt` files located in the `src/` and the `tests/` directories should not be changed.

### Helpers

In addition, the `dev/` folder contains custom helper scripts used to automatize the quality checks described above:

* `switch.sh` changes the `CMakeLists.txt` configuration to use
* `build.sh` compiles the program according to `CMakeLists.txt` in the root folder
* `run_tests.sh` runs all the tests and checks for errors
* `run_valgrind.sh` runs all the tests while analysing memory use
* `run_lcov.sh` runs all the tests and analyzed coverage
* `run_gprof.sh` runs the main program and analyzes performance

(See comments in the scripts for more details on how to use them.)

(Use `chmod +x ...` if needed to allow these scripts to run.) 

These scripts must be run from the root directory after the relevant executables have been built. Specifically, `run_tests.sh` and `run_valgrind.sh` require the test executables (e.g. configurations `tests.cmake` or `coverage.cmake`), while `run_lcov.sh` requires tests with coverage enabled (e.g. `coverage.cmake`) and `run_gprof.sh` requires the compiled program with profiling flags on (e.g. `profile.cmake`). So, make sure to have the right `CMakeLists.txt` file in the root folder before building.

Please note that these scripts are helper tools used on a Linux machine during development. As such, they **are not made to be compatible across platforms** and will require specific packages installed in order to run (e.g. Valgrind or LCOV).