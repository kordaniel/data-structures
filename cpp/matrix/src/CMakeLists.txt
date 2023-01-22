cmake_minimum_required(VERSION 3.16)

set(sources
  "Math.cpp"
  "Main.cpp"
  "Matrix.cpp"
  "Io.cpp"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set("EXECNAME" "${PROJECT_NAME}-dbg")
else()
    set("EXECNAME" "${PROJECT_NAME}")
endif()

add_executable("${EXECNAME}" "${sources}")
target_include_directories("${EXECNAME}" PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_compile_options("${EXECNAME}" PRIVATE "${CXX_FLAGS}"
    "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
    "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>"
)
