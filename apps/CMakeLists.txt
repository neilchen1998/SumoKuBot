add_executable(app app.cc)

# Link required libraries
target_link_libraries(app PRIVATE fmt::fmt board_library solver_library project_options)
