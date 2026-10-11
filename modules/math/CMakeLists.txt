find_package(Boost 1.67 QUIET)

if(Boost_FOUND)
  message(STATUS "Found Boost version: ${Boost_VERSION}")
else()
  message(STATUS "Boost not found or version is too old, downloading via FetchContent...")

  FetchContent_Declare(
    boost
    URL "https://archives.boost.io/release/1.84.0/source/boost_1_84_0.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

  FetchContent_MakeAvailable(boost)
endif()

add_library(boost_math_library INTERFACE)

target_include_directories(boost_math_library INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
  $<INSTALL_INTERFACE:include>
)

# Set _boost_target based on how Boost is installed
if(TARGET Boost::boost)
  set(_boost_target Boost::boost)
elseif(TARGET Boost::headers)
  set(_boost_target Boost::headers)
elseif(TARGET boost_headers)
  set(_boost_target boost_headers)
else()
  message(FATAL_ERROR "No suitable Boost headers target was found")
endif()

target_link_libraries(boost_math_library INTERFACE
  ${_boost_target}
  board_library
)
