project(wsac-dll CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)

add_library(wsac-dll SHARED ${SOURCES} ${HEADERS})
target_include_directories(wsac-dll PUBLIC src)
target_link_libraries(wsac-dll PUBLIC phnt)