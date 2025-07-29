cmake_minimum_required(VERSION 3.10)

# Project name
project(gitlite VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler flags for better debugging and warnings
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

# Collect all header files
file(GLOB_RECURSE HEADERS 
    "include/*.h"
)

# Create the main executable
add_executable(gitlite 
    main.cpp
    ${SOURCES}
)

# Link libraries if needed (for file system operations, etc.)
target_link_libraries(gitlite)

# Set output directory
set_target_properties(gitlite PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Copy testing directory to build directory (optional)
if(EXISTS ${CMAKE_SOURCE_DIR}/testing)
    file(COPY ${CMAKE_SOURCE_DIR}/testing 
         DESTINATION ${CMAKE_BINARY_DIR})
endif()

# Install target
install(TARGETS gitlite
    RUNTIME DESTINATION bin
)

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
else()
    message(STATUS "Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# Optional: Enable testing if CTest is available
enable_testing()

# Add a simple test target if you want to run the executable as a test
add_test(NAME gitlite_help_test 
         COMMAND gitlite 
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Set test to expect failure since running without arguments should show help
set_tests_properties(gitlite_help_test PROPERTIES WILL_FAIL TRUE)
