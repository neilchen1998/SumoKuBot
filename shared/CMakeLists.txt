find_package(Boost 1.67 QUIET)

if(Boost_FOUND)
  message(STATUS "Found Boost version: ${Boost_VERSION}")
else()
    message(STATUS "Boost not found or version is too old, downloading via FetchContent...")
    FetchContent_Declare(
      boost_hash
      GIT_REPOSITORY https://github.com/boostorg/container_hash.git
      GIT_TAG        boost-1.67.0
    )
    FetchContent_Declare(
      boost_core
      GIT_REPOSITORY https://github.com/boostorg/core.git
      GIT_TAG        boost-1.67.0
    )

    FetchContent_MakeAvailable(boost_hash boost_core)

    # Create an interface target so your code treats it like a system library
    add_library(Boost::boost INTERFACE IMPORTED)
    target_include_directories(Boost::boost INTERFACE
      ${boost_hash_SOURCE_DIR}/include
      ${boost_core_SOURCE_DIR}/include
    )
endif()

add_library(boost_math_library INTERFACE)

target_include_directories(boost_math_library INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>"
  "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(boost_math_library INTERFACE Boost::boost board_library project_options)
