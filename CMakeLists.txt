cmake_minimum_required(VERSION 3.0)

project(MiniSQL)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCE_FILES src/*.cpp src/*/*.cpp)

add_executable(main ${SOURCE_FILES})

# add_executable(miniSQL ${SOURCE_FILES})

