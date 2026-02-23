file(GLOB BOARD_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/board/*.hpp")
file(GLOB SOLVER_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/solver/*.hpp")

find_package(Boost REQUIRED)

add_library(board_library INTERFACE)
add_library(solver_library INTERFACE)

target_include_directories(board_library INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
)

target_include_directories(solver_library INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
)

target_link_libraries(board_library INTERFACE  Boost::boost fmt::fmt)
target_link_libraries(solver_library INTERFACE fmt::fmt)
