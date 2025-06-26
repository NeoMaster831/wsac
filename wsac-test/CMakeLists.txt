project(wsac-test CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)

add_executable(wsac-test ${SOURCES} ${HEADERS})
add_dependencies(wsac-test wsac-dll)
