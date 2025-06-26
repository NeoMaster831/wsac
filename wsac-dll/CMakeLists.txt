project(wsac-dll CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)

configure_file(src/Config.hpp.in src/Config.hpp @ONLY)

add_library(wsac-dll SHARED ${SOURCES} ${HEADERS})
target_include_directories(wsac-dll PUBLIC src)
target_include_directories(wsac-dll PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/src")
target_link_libraries(wsac-dll PUBLIC phnt cfproxy)
