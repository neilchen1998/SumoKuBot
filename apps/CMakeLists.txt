find_package(cli11 CONFIG 2.4.1 QUIET)

if (cli11_FOUND)
    message(STATUS "Found cli11 version: ${cli11_VERSION}")
else()
    message(STATUS "cli11 could not be found or version is too old, downloading via FetchContent...")
    FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.4.1
    )
    FetchContent_MakeAvailable(cli11)
endif()

add_executable(app app.cc)

# Link required libraries
target_link_libraries(app PRIVATE CLI11::CLI11 fmt::fmt board_library solver_library loader_library project_options)
