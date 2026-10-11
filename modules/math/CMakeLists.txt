include(FetchContent)

set(BOOST_VERSION 1.67.0)
set(BOOST_INSTALL_DIR "${CMAKE_BINARY_DIR}/boost-install")

find_package(Boost ${BOOST_VERSION} QUIET)

if(Boost_FOUND)
    message(STATUS "Found Boost version: ${Boost_VERSION}")
else()
    message(STATUS "Boost not found. Downloading full Boost ${BOOST_VERSION}...")

    include(ExternalProject)

    ExternalProject_Add(boost_external
        URL https://archives.boost.io/release/1.67.0/source/boost_1_67_0.tar.gz

        PREFIX "${CMAKE_BINARY_DIR}/boost-build"

        CONFIGURE_COMMAND
            <SOURCE_DIR>/bootstrap.sh
            --prefix=${BOOST_INSTALL_DIR}

        BUILD_COMMAND
            <SOURCE_DIR>/b2
            install
            --prefix=${BOOST_INSTALL_DIR}
            -j4

        INSTALL_COMMAND ""

        BUILD_IN_SOURCE TRUE
    )

    set(Boost_ROOT "${BOOST_INSTALL_DIR}")
    set(Boost_INCLUDE_DIR "${BOOST_INSTALL_DIR}/include")
    set(Boost_LIBRARY_DIR "${BOOST_INSTALL_DIR}/lib")

    message(STATUS "Boost will be installed to: ${BOOST_INSTALL_DIR}")

    # Ensure Boost is built and installed before your target builds.
    add_custom_target(install_boost DEPENDS boost_external)
endif()

add_library(boost_math_library INTERFACE)

target_include_directories(boost_math_library INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(boost_math_library INTERFACE
  Boost::boost
  board_library
)
