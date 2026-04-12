find_package(nlohmann_json 3.9.1 REQUIRED)

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

add_library(loader_library STATIC ${PROJECT_SOURCE_DIR}/shared/loaderlib.cc)

# Create a macro called TEST_DATA_DIR
target_compile_definitions(loader_library PRIVATE
    TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data"
)

target_include_directories(loader_library PUBLIC ${CMAKE_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR})
target_link_libraries(loader_library PUBLIC nlohmann_json::nlohmann_json board_library math_library)
