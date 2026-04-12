# The minimum cmake requirements
cmake_minimum_required(VERSION 3.18...4.0)

project(
    Sudoku
    VERSION 1.0
    DESCRIPTION "Solve Sudoku"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if we are at the top level
set(PROJECT_IS_TOP_LEVEL OFF)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(PROJECT_IS_TOP_LEVEL ON)
endif()

# Check if we are at the top level
if(PROJECT_IS_TOP_LEVEL)

    # Ensure -std=c++xx instead of -std=g++xx for protability purposes
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Set the default build type if not provided
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "No build type specified. Set to 'Release' by default.")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    endif()

    # Compiler flags
    add_library(project_options INTERFACE)

    # Flags for all build types
    target_compile_options(project_options INTERFACE -Wall -Wextra -Wpedantic)

    # Flags based on the specified build type
    target_compile_options(project_options INTERFACE
        $<$<CONFIG:Release>:-O3 -march=native -ffast-math -DNDEBUG>
        $<$<CONFIG:Debug>:-g -O0>
    )

    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT error)

    if(lto_supported)
        # Enable LTO specifically for the Benchmark configuration
        set_target_properties(project_options PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_RELEASE   TRUE
            INTERPROCEDURAL_OPTIMIZATION_BENCHMARK TRUE
        )
        message(STATUS "LTO/IPO is supported and enabled")
    else()
        message(STATUS "LTO/IPO not supported: ${error}")
    endif()

    # Linker flags
    target_link_options(project_options INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address -fsanitize=undefined>
    )

    # Docs only available in the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found")
    endif()
endif()

# FetchContent helps grabbing repos during the configure step
# NOTE: available after CMake 3.11
include(FetchContent)

# all required packages (will be downloaded automatically)

# Try to find fmt that is at least 12.0 but the same major version
find_package(fmt CONFIG 12.0...<13.0 QUIET)
if (fmt_FOUND)
    message(STATUS "Found fmt version: ${fmt_VERSION}")
else()
    message(STATUS "fmt could not be found or version is too old, downloading via FetchContent...")
    FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        12.1.0)
    FetchContent_MakeAvailable(fmt)
endif()

# The compiled library code
add_subdirectory(src)

# The executable code
add_subdirectory(apps)

option(PROJECT_BUILD_TESTS "Build tests" OFF)
option(PROJECT_BUILD_BENCHMARKS "Build benchmarks" OFF)

# Always include shared if we're top-level or explicitly requested
if(PROJECT_IS_TOP_LEVEL OR PROJECT_BUILD_TESTS OR PROJECT_BUILD_BENCHMARKS)

    # This subdirectory contains files needed by tests and benchmarks
    add_subdirectory(shared)

    if(PROJECT_BUILD_TESTS)
        # Use CTest and set BUILD_TESTING
        include(CTest)
        if(BUILD_TESTING)
            add_subdirectory(tests)
        endif()
    endif()

    if(PROJECT_BUILD_BENCHMARKS)
        add_subdirectory(benchmarks)
    endif()
endif()
