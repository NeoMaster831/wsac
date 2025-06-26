cmake_minimum_required(VERSION 3.31)
project(wsac)

set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(WDK_PFX "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestSigning.pfx")

set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_IMPORT_LIBRARY_PREFIX "")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build")

add_subdirectory(sub/phnt)
add_subdirectory(sub/cfproxy)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/SetConfigIn.cmake")

add_subdirectory(wsac-dll)
add_subdirectory(wsac-krn)
add_subdirectory(wsac-server)
add_subdirectory(wsac-service)

add_subdirectory(wsac-test)
