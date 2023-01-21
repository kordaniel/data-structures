# P1-matrix-multiplication
Provides operations for (2D) matrices.

## Requirements
- cmake 3.16+
- C++17 compiler and build tools

## Build & Run
```console
foo@bar:$ cmake -S. -B build
foo@bar:$ cmake --build build
foo@bar:$ ./build/Matrixer
```

## Tests
Generate test matrices into `test/data/` (requires python3 & numpy).
```
foo@bar:$ cd test/
foo@bar:$ ./generate-test-matrices.py
```
