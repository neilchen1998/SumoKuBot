# Create a CMake library target
add_library(board_library INTERFACE)

target_include_directories(board_library INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Link all the libraries & flags that are required
target_link_libraries(board_library INTERFACE fmt::fmt nlohmann_json::nlohmann_json)
target_link_libraries(board_library INTERFACE project_options)
