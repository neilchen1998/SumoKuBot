file(GLOB BOARD_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/board/*.hpp")
file(GLOB MATH_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/math/*.hpp")
file(GLOB SOLVER_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/solver/*.hpp")

find_package(nlohmann_json 3 QUIET)

if (nlohmann_json_FOUND)
  message(STATUS "Found nlohmann_json version: ${nlohmann_json_VERSION}")
else()
  message(STATUS "nlohmann_json not found or version is too old, downloading via FetchContent...")
  FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.9.1
  )
  FetchContent_MakeAvailable(json)
endif()

add_library(board_library INTERFACE)
add_library(math_library INTERFACE)
add_library(solver_library INTERFACE)
add_library(loader_library STATIC ${PROJECT_SOURCE_DIR}/src/loaderlib.cc)

# Create a macro called TEST_DATA_DIR
target_compile_definitions(loader_library PRIVATE
    TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data"
)

# Include the directories
target_include_directories(board_library INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
)
target_include_directories(loader_library PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
)
target_include_directories(math_library INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
)
target_include_directories(solver_library INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
)

# Link all the libraries & flags that are required
target_link_libraries(board_library INTERFACE fmt::fmt project_options)
target_link_libraries(loader_library PUBLIC nlohmann_json::nlohmann_json board_library math_library project_options)
target_link_libraries(math_library INTERFACE project_options)
target_link_libraries(solver_library INTERFACE fmt::fmt math_library project_options)
