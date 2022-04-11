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

Below are some early benchmark numbers. I think the absolute values of these benchmarks are less important than the values relative to one another: the flat map implementation provides a ~15% speed-up, whereas finding an order's queue position is quite slow relative to other operations, due to the O(N) search of a linked-list - a lot of cache misses.

<p align="center">
  <img src="https://github.com/Hnodomar/technical-question/blob/main/description/benchmarks.jpg" alt="Benchmarks"/>
</p>