find_package(Boost 1.67 QUIET)

if(Boost_FOUND)
  message(STATUS "Found Boost version: ${Boost_VERSION}")
else()
  message(STATUS "Boost not found or version is too old, downloading via FetchContent...")

  FetchContent_Declare(
    boost
    URL "https://github.com/boostorg/boost/releases/download/boost-1.92.0/boost-1.92.0-cmake.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

  FetchContent_MakeAvailable(boost)
endif()

# Fallback: Ensure Boost::boost exists if FetchContent was used
if(NOT TARGET Boost::boost)
  if(TARGET Boost::headers)
    add_library(Boost::boost ALIAS Boost::headers)
  elseif(TARGET boost_headers)
    add_library(Boost::boost ALIAS boost_headers)
  else()
    add_library(Boost::boost INTERFACE)
  endif()
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
