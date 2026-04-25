# The minimum cmake requirements
cmake_minimum_required(VERSION 3.21...4.0)

project(
    SumoKuBot
    VERSION 2.0.0
    DESCRIPTION "Solve Sudoku puzzles"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure -std=c++xx instead of -std=g++xx for protability purposes
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
add_library(project_options INTERFACE)

# Options
option(PROJECT_BUILD_TESTS "Build tests" OFF)
option(PROJECT_BUILD_BENCHMARKS "Build benchmarks" OFF)

# Check if we are at the top level
# NOTE: PROJECT_IS_TOP_LEVEL is available after CMake 3.21
if(PROJECT_IS_TOP_LEVEL)

    # Set the default build type if not provided
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "No build type specified. Set to 'Release' by default.")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    endif()

    # Flags for all build types
    target_compile_options(project_options INTERFACE -Wall -Wextra -Wpedantic)

    # Flags based on the specified build type
    target_compile_options(project_options INTERFACE
        # High performance builds
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-O3 -march=native -ffast-math -DNDEBUG>
        $<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>

        # Debug builds
        $<$<CONFIG:Debug>:-g -O0 -fsanitize=address,undefined>
        $<$<CONFIG:RelWithDebInfo>:-g>
    )

    # Check if IPO/LTO is supported
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT error)
    if(lto_supported)
        # Enable LTO specifically for release builds
        set_target_properties(project_options PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_RELEASE        TRUE
            INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
            INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL     TRUE
        )
        message(STATUS "LTO/IPO is supported and enabled")
    else()
        message(STATUS "LTO/IPO not supported: ${error}")
    endif()

    # Linker flags
    target_link_options(project_options INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )

    # Docs only available in the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found")
    endif()

    # Build date
    string(TIMESTAMP PROJECT_BUILD_DATE "%d-%b-%Y")
endif()

# FetchContent helps grabbing repos during the configure step
# NOTE: available after CMake 3.11
include(FetchContent)

# All required packages will be downloaded automatically if unavailable

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

# Configure the SW version file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/project/version.h.in"
    "${CMAKE_BINARY_DIR}/generated/version.h"
    @ONLY
)

# The compiled library subdirectory
add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
    # The executable subdirectory
    add_subdirectory(apps)
endif()

# Optional modules
if(PROJECT_IS_TOP_LEVEL AND (PROJECT_BUILD_TESTS OR PROJECT_BUILD_BENCHMARKS))

    # This subdirectory contains files needed by tests and benchmarks
    add_subdirectory(shared)

    # The unit test subdirectory
    if(PROJECT_BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()

    # The benchmarking subdirectory
    if(PROJECT_BUILD_BENCHMARKS)
        add_subdirectory(benchmarks)
    endif()
endif()

if(PROJECT_IS_TOP_LEVEL)

    # The name of the package <PROJECT_NAME>-<PROJECT_VERSION>.tar.gz
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

    # The location of LICENSE
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

    # The location of README
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    # Bundle the package into a compressed tarball
    set(CPACK_GENERATOR "TGZ")

    # Generate SHA256 for users to verify the integrity of the package
    set(CPACK_PACKAGE_CHECKSUM "SHA256")

    # Install LICENSE and README into the package
    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        DESTINATION .
    )

    # CPack helps
    include(CPack)
endif()
