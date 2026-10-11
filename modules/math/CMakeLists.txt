include(FetchContent)

set(BOOST_VERSION 1.67.0)
set(BOOST_INSTALL_DIR "${CMAKE_BINARY_DIR}/boost-install")

find_package(Boost ${BOOST_VERSION} QUIET)

FetchContent_Declare(
    boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(boost)

add_library(boost_math_library INTERFACE)

target_include_directories(boost_math_library INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(boost_math_library INTERFACE
  Boost::boost
  board_library
)
