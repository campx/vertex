cmake_minimum_required(VERSION 2.8)

project(vertex LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -Wall -Wextra -Werror")
include_directories(${vertex_SOURCE_DIR}/include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #create compilation database for clang

add_library(libvertex
    include/vertex/Link.h                   src/Link.cpp
    include/vertex/Vertex.h                 src/Vertex.cpp
    include/vertex/ManagedVertexMap.h       src/ManagedVertexMap.cpp
    include/vertex/EdgeIterator.h           src/EdgeIterator.cpp
    include/vertex/BredthFirstTraversal.h   src/BredthFirstTraversal.cpp
    include/vertex/PreOrderTraversal.h      src/PreOrderTraversal.cpp
    include/vertex/InOrderTraversal.h       src/InOrderTraversal.cpp
    include/vertex/Traversal.h              src/Traversal.cpp
    include/vertex/PostOrderTraversal.h     src/PostOrderTraversal.cpp
    include/vertex/VertexIterator.h         src/VertexIterator.cpp
    include/vertex/Iterator.h               src/Iterator.cpp)

set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)

add_subdirectory(external/gtest)

target_include_directories(libvertex INTERFACE "${vertex_SOURCE_DIR}/include")

enable_testing()
add_executable(TestVertex
               test/Link.cpp
               test/Vertex.cpp
               test/Traversal.cpp
               test/Iterator.cpp
               test/main.cpp
              )
target_link_libraries(TestVertex gtest libvertex)
set_target_properties(TestVertex PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestVertex
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestVertex)
