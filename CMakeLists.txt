# The minimum cmake requirements
cmake_minimum_required(VERSION 3.24...4.0)

project(
  SumoKuBot
  VERSION 2.1.0
  DESCRIPTION "A fast C++ CLI that solves Sumoku and Killer Sudoku puzzles"
  LANGUAGES CXX
)

# Options
option(PROJECT_BUILD_APPS "Build applications" ON)
option(PROJECT_BUILD_TESTS "Build tests" OFF)
option(PROJECT_BUILD_BENCHMARKS "Build benchmarks" OFF)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure -std=c++xx instead of -std=g++xx for portability purposes
set(CMAKE_CXX_EXTENSIONS OFF)

# Check if we are at the top level
if(PROJECT_IS_TOP_LEVEL)

  # Set the default build type if not provided
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified. Set to 'Release' by default.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  endif()

  # Compiler flags
  add_library(project_options INTERFACE)

  # Flags for all build types
  target_compile_options(project_options INTERFACE
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wextra>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wpedantic>
  )

  # Flags based on the specified build type
  target_compile_options(project_options INTERFACE
    # High performance builds
    $<$<CONFIG:Release>:-march=native>
    $<$<CONFIG:RelWithDebInfo>:-march=native>

    # Debug builds
    $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>
  )

  # Linker flags
  target_link_options(project_options INTERFACE
    $<$<AND:$<CONFIG:Debug>,$<LINK_LANG_AND_ID:CXX,GNU,Clang,AppleClang>>:-fsanitize=address,undefined>
  )

  # Check if IPO/LTO is supported
  include(CheckIPOSupported)
  check_ipo_supported(RESULT lto_supported OUTPUT error)
  if(lto_supported)
    # Enable LTO specifically for release builds
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
    message(STATUS "LTO/IPO is supported and enabled")
  else()
    message(STATUS "LTO/IPO not supported: ${error}")
  endif()

  # Docs only available in the main app
  find_package(Doxygen QUIET)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found")
  endif()

  # Build date
  string(TIMESTAMP PROJECT_BUILD_DATE "%d-%b-%Y")
endif()

# FetchContent helps grabbing repos during the configure step
include(FetchContent)

# All required packages will be downloaded automatically if unavailable
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        12.1.0
  FIND_PACKAGE_ARGS CONFIG 12.0...<13.0
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.16.0
  FIND_PACKAGE_ARGS CONFIG 1.16.0
)
FetchContent_MakeAvailable(spdlog)

find_package(nlohmann_json 3 QUIET)

if (nlohmann_json_FOUND)
  message(STATUS "Found nlohmann_json version: ${nlohmann_json_VERSION}")
else()
  message(STATUS "nlohmann_json not found or version is too old, downloading via FetchContent...")
  FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
  )
  FetchContent_MakeAvailable(json)
endif()

# Configure the SW version file
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/modules/project/version.h.in"
  "${CMAKE_BINARY_DIR}/generated/version.h"
  @ONLY
)

# The compiled library subdirectory
add_subdirectory(modules/board)
add_subdirectory(modules/loader)
add_subdirectory(modules/math)
add_subdirectory(modules/solvers/sudoku)
add_subdirectory(modules/solvers/sumoku)

if(PROJECT_IS_TOP_LEVEL AND PROJECT_BUILD_APPS)
  # The executable subdirectory
  add_subdirectory(apps)
endif()

# Optional modules
if(PROJECT_IS_TOP_LEVEL AND (PROJECT_BUILD_TESTS OR PROJECT_BUILD_BENCHMARKS))

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
