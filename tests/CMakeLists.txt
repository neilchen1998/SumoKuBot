# Try to find Catch2 that is at least 3.8 but the same major version
find_package(Catch2 CONFIG 3.8...<4.0 QUIET)
if (Catch2_FOUND)
  message(STATUS "Found Catch2 version: ${Catch2_VERSION}")
else()
  message(STATUS "Catch2 could not be found or version is too old, downloading via FetchContent...")
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1)
  FetchContent_MakeAvailable(Catch2)
endif()

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(solvertestlib solvertestlib.cc)
add_executable(boardtestlib boardtestlib.cc)

target_link_libraries(solvertestlib PRIVATE Catch2::Catch2WithMain solver_library project_options)
target_link_libraries(boardtestlib PRIVATE Catch2::Catch2WithMain solver_library project_options)

catch_discover_tests(solvertestlib)
catch_discover_tests(boardtestlib)
