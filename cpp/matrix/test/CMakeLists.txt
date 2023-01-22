cmake_minimum_required(VERSION 3.16)

add_compile_options("${CXX_FLAGS}" "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>")
add_link_options("$<$<CONFIG:Debug>:${CXX_LDFLAGS_DEBUG}>")

include_directories(
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_SOURCE_DIR}/src"
)

link_libraries(gtest gmock gtest_main)

set(MatrixTest "MatrixTest")
set(MatrixTextSources
    "MatrixTest.cpp"
)
add_executable("${MatrixTest}" "${MatrixTextSources}")
add_test(
    NAME "${MatrixTest}"
    COMMAND "${MatrixTest}"
)
