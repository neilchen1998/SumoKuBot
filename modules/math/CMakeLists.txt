option(USE_FETCHED_BOOST "Fetch Boost instead of using system Boost" OFF)

add_library(boost_math_library INTERFACE)

if(NOT USE_FETCHED_BOOST)
  find_package(Boost 1.84 QUIET)
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
else()
  message(STATUS "Fetching Boost 1.84.0")

  include(FetchContent)

  # Avoid accidentally resolving the dependency to a system Boost package.
  set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)

  FetchContent_Declare(
    Boost
    URL "https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz"
    URL_HASH MD5=893b5203b862eb9bbd08553e24ff146a
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

  FetchContent_MakeAvailable(Boost)

  if(TARGET Boost::headers)
    target_link_libraries(boost_math_library INTERFACE Boost::headers)
  elseif(TARGET Boost::boost)
    target_link_libraries(boost_math_library INTERFACE Boost::boost)
  else()
    message(FATAL_ERROR "Fetched Boost has no headers target")
  endif()
endif()

target_include_directories(boost_math_library INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(boost_math_library INTERFACE
  board_library
)
