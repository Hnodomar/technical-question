find_package(Catch2 QUIET)

if (${Catch2_FOUND})
  message(STATUS "Successfully found Catch2")
  include(Catch)
else()
  message(STATUS "Fetching Catch2 unit testing framework")
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v2.13.8
    CMAKE_ARGS
      -DCATCH_INSTALL_DOCS=off
      -DCATCH_INSTALL_EXTRAS=off
      -DCATCH_BUILD_TESTING=off
      -DCPPZMQ_BUILD_TESTS=off
  )
  FetchContent_MakeAvailable(Catch2)
  # ugly workaround to avoid unwanted installation
  # Catch2 used to be a header-only library but that's changed (unfortunately)
  include("${CMAKE_BINARY_DIR}/_deps/catch2-src/contrib/Catch.cmake")
endif(${Catch2_FOUND})

add_executable(unit_tests unit_tests.cpp)
target_link_libraries(unit_tests Catch2::Catch2)


target_include_directories(unit_tests PRIVATE 
  ${datafeed_inc}
  ${Boost_INCLUDE_DIR}
)

target_link_libraries(unit_tests ${Boost_LIBRARIES})

catch_discover_tests(unit_tests)