# LidyaTech Challenge

Please note that a Boost and CMake installation are required to compile this project. 

## Usage

From the project directory,

```
mkdir build
cd build
cmake -DBUILD_TESTS=ON ..
make
ctest -V
```

will compile and run the tests.

CMake will fetch and build Catch2 and Google Benchmark for the project, but I have configured both of these dependencies in cmake to not install on the users system. They'll remain local to the project.

### Running Benchmarks

After following the above build pattern:

```
cd tests
cd benchmark
./benchmarks
```

## Benchmarks

Below are the results of some rough benchmarking that I did, with suprising results.

I expected the flat map to provide a noticable speed-up relative to the RB-tree of the STL. Instead, they're similar. Needs investigation.

The flat map is expectedly faster in retrieving queue positions, since this requires iterating through the map.

<p align="center">
  <img src="https://github.com/Hnodomar/technical-question/blob/main/description/benchmarks.jpg" alt="Benchmarks"/>
</p>