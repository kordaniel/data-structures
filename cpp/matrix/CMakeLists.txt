cmake_minimum_required(VERSION 3.16)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL ERROR: In-source builds are not allowed.
             Specify a separate build dir with the -B flag for build files.
")
endif()

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

project(Matrixer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wconversion -pedantic -Wpedantic -Wunused-result -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wsign-conversion -fdelete-null-pointer-checks -Wnull-dereference -Wdouble-promotion")

# Set flags for apple clang
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    add_compile_options("-Wshadow-field-in-constructor" "-Wsign-compare")
endif()

# Append RELEASE / DEBUG specific flags
list(APPEND CXX_FLAGS_RELEASE "-Werror" "-flto") # -O3 -DNDEBUG is set by cmake
#list(APPEND CXX_FLAGS_DEBUG "-fsanitize=undefined" "-fsanitize=address" "-O0") # -g is set by cmake

set(sources
  "${CMAKE_SOURCE_DIR}/src/Math.cpp"
  "${CMAKE_SOURCE_DIR}/src/Main.cpp"
  "${CMAKE_SOURCE_DIR}/src/Matrix.cpp"
  "${CMAKE_SOURCE_DIR}/src/Io.cpp"
)

add_executable(Matrixer "${sources}")
target_include_directories(Matrixer PRIVATE "${CMAKE_SOURCE_DIR}/include")

