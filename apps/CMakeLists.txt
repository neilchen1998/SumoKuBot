add_executable(app app.cc)

# Link required libraries
target_link_libraries(app PRIVATE fmt::fmt)
