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

# Install the binary into the 'bin' directory
install(TARGETS app DESTINATION bin)

# Show the binary size after it is built
add_custom_command(
    TARGET app
    POST_BUILD
    COMMAND size $<TARGET_FILE:app>
    COMMENT "Display binary size..."
    VERBATIM
)

# Print out the version from the binary after it is built
add_custom_command(
    TARGET app
    POST_BUILD
    COMMAND $<TARGET_FILE:app> --version
    COMMENT "Verifying binary runs and reports the SW version..."
)
