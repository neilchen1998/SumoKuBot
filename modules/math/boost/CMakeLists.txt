include(FetchContent)

option(USE_FETCHED_BOOST "Fetch Boost instead of using system Boost" OFF)

add_library(boost_math_library INTERFACE)

if(NOT USE_FETCHED_BOOST)
  find_package(Boost 1.78 QUIET)
endif()

if(Boost_FOUND AND NOT USE_FETCHED_BOOST)
  message(STATUS "Using system Boost ${Boost_VERSION}")

  if(TARGET Boost::headers)
    target_link_libraries(boost_math_library INTERFACE Boost::headers)
  elseif(TARGET Boost::boost)
    target_link_libraries(boost_math_library INTERFACE Boost::boost)
  else()
    target_include_directories(boost_math_library SYSTEM INTERFACE
      ${Boost_INCLUDE_DIRS}
    )
  endif()

  if(TARGET Boost::container_hash)
    target_link_libraries(boost_math_library INTERFACE
      Boost::container_hash
    )
  endif()

else()
  message(STATUS "Fetching Boost 1.84.0")

  set(BOOST_INCLUDE_LIBRARIES container_hash)
  set(BOOST_ENABLE_CMAKE ON)

  FetchContent_Declare(
    Boost
    URL "https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz"
    URL_HASH MD5=893b5203b862eb9bbd08553e24ff146a
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

  FetchContent_MakeAvailable(Boost)

  if(NOT TARGET Boost::container_hash)
    message(FATAL_ERROR "Boost.ContainerHash target is missing")
  endif()

  target_link_libraries(boost_math_library INTERFACE
    Boost::container_hash
  )
endif()

target_include_directories(boost_math_library INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(boost_math_library INTERFACE
  board_library
)
