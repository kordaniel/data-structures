cmake_minimum_required(VERSION 3.16)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
FATAL ERROR: In-source builds are not allowed.
             Specify a separate build dir with the -B flag for build files.
")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

project(Matrixer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
#set(THREADS_PREFER_PTHREAD_FLAG_TRUE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wconversion -pedantic -Wpedantic -Wunused-result -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wsign-conversion -fdelete-null-pointer-checks -Wnull-dereference -Wdouble-promotion")

# Set flags for apple clang
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    add_compile_options("-Wshadow-field-in-constructor" "-Wsign-compare")
endif()

# Append RELEASE / DEBUG specific flags
list(APPEND CXX_FLAGS_RELEASE "-Werror" "-flto") # -O3 -DNDEBUG is set by cmake
#list(APPEND CXX_FLAGS_DEBUG "-fsanitize=undefined" "-fsanitize=address" "-O0") # -g is set by cmake

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/6b74da4757a549563d7c37c8fae3e704662a043b.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_testing()
    add_subdirectory("${PROJECT_SOURCE_DIR}/test")
endif()
