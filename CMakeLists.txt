cmake_minimum_required(VERSION 3.16)

project(TechChallenge DESCRIPTION "Technical challenge for LidyaTech")

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(BOOST_MIN_VERSION "1.70.0")

include(ExternalProject)
include(FetchContent)

find_package(Boost COMPONENTS iostreams thread)

set(BENCHMARK_ENABLE_TESTING OFF)

if (${Boost_FOUND})
  message(STATUS "Successfully found Boost")
else()
  ExternalProject_Add(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost
    GIT_TAG boost-1.78.0
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
  )
endif(${Boost_FOUND})

file(GLOB datafeed_inc
  ${CMAKE_SOURCE_DIR}/include
)

if (${BUILD_TESTS})
  enable_testing()
  add_subdirectory(tests)
endif()