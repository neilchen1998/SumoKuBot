find_package(CLI11 CONFIG 2.4.1 QUIET)

if (CLI11_FOUND)
    message(STATUS "Found CLI11 version: ${CLI11_VERSION}")
else()
    message(STATUS "CLI11 could not be found or version is too old, downloading via FetchContent...")
    FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.4.1
    )
    FetchContent_MakeAvailable(CLI11)
endif()

add_executable(app app.cc)

target_include_directories(app PRIVATE "${CMAKE_BINARY_DIR}/generated")

# Link required libraries
target_link_libraries(app PRIVATE CLI11::CLI11 fmt::fmt board_library solver_library loader_library project_options)
