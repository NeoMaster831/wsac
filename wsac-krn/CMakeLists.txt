cmake_minimum_required(VERSION 3.31)
project(wsac-krn CXX)

find_package(WDK REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)

wdk_add_driver(wsac-krn)
target_sources(wsac-krn PRIVATE ${SOURCES} ${HEADERS})
