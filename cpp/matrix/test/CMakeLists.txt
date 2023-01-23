cmake_minimum_required(VERSION 3.16)

add_compile_options("${CXX_FLAGS}" "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>")
add_link_options("$<$<CONFIG:Debug>:${CXX_LDFLAGS_DEBUG}>")

include_directories(
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)

link_libraries(gtest gmock gtest_main)

set(TestMatrix "TestMatrix")
set(TestMatrixSources
    "MatrixTest.cpp"
    "TestHelpers.cpp"
    "${CMAKE_SOURCE_DIR}/src/Io.cpp"
    "${CMAKE_SOURCE_DIR}/src/Math.cpp"
    "${CMAKE_SOURCE_DIR}/src/Matrix.cpp"
)
add_executable("${TestMatrix}" "${TestMatrixSources}")
add_test(
    NAME "${TestMatrix}"
    COMMAND "${TestMatrix}"
)
