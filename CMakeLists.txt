cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Crush20L VERSION 1.0)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable debugging symbols and disable optimizations for Debug build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")  # Debug flags: generate debug symbols, no optimizations

# Set optimization flags for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find PAPI library manually (if not automatically found)
find_library(PAPI_LIBRARY NAMES papi)

if (NOT PAPI_LIBRARY)
    message(FATAL_ERROR "PAPI library not found!")
endif()

# All your source files
set(SOURCES
    main.cpp
    Stage.cpp
    Stage1.cpp
    Stage2.cpp
    Stage3.cpp
    Stage4.cpp
    Stage5.cpp
    Stage6.cpp
    ParameterQueue.hpp
    CircularBuffer.hpp
    ReSample.hpp
)

# Add the executable
add_executable(Crush20L ${SOURCES})

# Link PAPI if it was found
target_link_libraries(Crush20L PRIVATE ${PAPI_LIBRARY})


# Optionally, set up PAPI for performance monitoring (for example)
# You would include PAPI headers and use them in your code for performance metrics

# Enable sanitizers (optional, useful for debugging memory issues)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined")
endif()
