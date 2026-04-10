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

# Check if we are in the main CMakeLists
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

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
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Benchmark>:-O3 -march=native -ffast-math -DNDEBUG>
        $<$<CONFIG:Test>:-g -O1 -fsanitize=address -fsanitize=undefined>
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
        $<$<CONFIG:Test>:-fsanitize=address -fsanitize=undefined>
    )

    # Use CTest
    # NOTE: this needs to be done in the main CMakeLists
    include(CTest)

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

# Check if we are in the main CMakeLists
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(tests)
    if(CMAKE_BUILD_TYPE STREQUAL "Benchmark")
        add_subdirectory(benchmarks)
    endif()
endif()
