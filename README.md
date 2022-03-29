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
