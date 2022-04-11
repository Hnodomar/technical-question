find_package(benchmark QUIET)

if (${benchmark_FOUND})
  message(STATUS "Successfully found Google Benchmark")
else()
  message(STATUS "Fetching Google Benchmark")
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
    CMAKE_ARGS -DINSTALL_GTEST=off -DBENCHMARK_ENABLE_TESTING=OFF -DCMAKE_BUILD_TYPE=Release
  )
  FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark
    GIT_TAG v1.6.1
    CMAKE_ARGS 
      -DCMAKE_BUILD_TYPE=Release
      -DBENCHMARK_ENABLE_TESTING=OFF 
      -DGOOGLETEST_PATH="${CMAKE_BINARY_DIR}/_deps/googletest-src/" 
      -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on 
      -DBENCHMARK_ENABLE_INSTALL=off 
      -DBENCHMARK_INSTALL_DOCS=off
  )
  FetchContent_MakeAvailable(googletest benchmark)
endif(${benchmark_FOUND})

add_executable(benchmarks
  benchmark.cpp
)

target_include_directories(benchmarks PRIVATE 
  ${datafeed_inc}
  ${Boost_INCLUDE_DIR}
)

target_link_libraries(benchmarks 
  benchmark::benchmark 
  ${Boost_LIBRARIES}
)

