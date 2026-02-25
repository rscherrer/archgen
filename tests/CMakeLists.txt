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