# Create a CMake library target
add_library(loader_library loaderlib.cc)

# Create a macro called TEST_DATA_DIR
target_compile_definitions(loader_library PRIVATE
    TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data"
)

# Include the directories
target_include_directories(loader_library PUBLIC
  ${PROJECT_SOURCE_DIR}/modules
)

# Link all the libraries & flags that are required
target_link_libraries(loader_library PRIVATE nlohmann_json::nlohmann_json board_library math_library)
target_link_libraries(loader_library PRIVATE project_options)
