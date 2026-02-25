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