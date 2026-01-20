cmake_minimum_required(VERSION 3.28)

project(PART2 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/bin)

find_package(PkgConfig REQUIRED)
pkg_search_module(PKG_ZLIB REQUIRED IMPORTED_TARGET zlib)
find_package(OpenCV REQUIRED)

add_subdirectory(libs)
add_subdirectory(src)
