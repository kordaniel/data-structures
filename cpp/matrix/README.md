# P1-matrix-multiplication
Provides operations for (2D) matrices.

## Requirements
- cmake 3.16+
- C++17 compiler and build tools

## Build & Run
```console
foo@bar:$ cmake -S. -B build/release
foo@bar:$ cmake --build build/release
foo@bar:$ ./bin/Matrixer

foo@bar:$ cmake -DCMAKE_BUILD_TYPE=Debug -S. -B build/debug
foo@bar:$ cmake --build build/debug
foo@bar:$ ./bin/Matrixer-dbg
```

## Tests
Generate test matrices into `test/data/` (requires python3 & numpy).
```console
foo@bar:$ cd test/
foo@bar:$ ./generate-test-matrices.py
```
