include(FetchContent)
include(GNUInstallDirs)

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

find_package(magic_enum CONFIG 0.9.8 QUIET)

if (magic_enum_FOUND)
    message(STATUS "Found magic_enum version: ${magic_enum_VERSION}")
else()
    message(STATUS "magic_enum could not be found or version is too old, downloading via FetchContent...")
    FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG        v0.9.8
    )
    FetchContent_MakeAvailable(magic_enum)
endif()

add_library(cli cli/arguments.cc)

add_executable(app app.cc)

target_include_directories(app PRIVATE "${CMAKE_BINARY_DIR}/generated")

# Link required libraries
target_link_libraries(cli PRIVATE magic_enum::magic_enum)

target_link_libraries(cli INTERFACE project_options)

target_link_libraries(app PRIVATE
  board_library
  CLI11::CLI11
  fmt::fmt
  loader_library
  spdlog::spdlog

  # targets
  sumoku::solver
  cli
)

target_link_libraries(app PRIVATE project_options)

# Install the binary into the 'bin' directory
install(TARGETS app
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Show the binary size after it is built
add_custom_command(
    TARGET app
    POST_BUILD
    COMMAND size $<TARGET_FILE:app>
    COMMENT "Display binary size..."
    VERBATIM
)
