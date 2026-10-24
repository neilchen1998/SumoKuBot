# Create a CMake library target
add_library(math_library INTERFACE)

# Expose the 'modules' directory to targets that link against this library
target_include_directories(math_library INTERFACE
  ${PROJECT_SOURCE_DIR}/modules
)

# Link all the libraries & flags that are required
target_link_libraries(math_library INTERFACE board_library)
target_link_libraries(math_library INTERFACE project_options)
