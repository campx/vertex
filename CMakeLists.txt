cmake_minimum_required(VERSION 2.8)

project(vertex LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
include_directories(${vertex_SOURCE_DIR}/include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #create compilation database for clang

add_library(libvertex
            src/Vertex.cpp
            src/VertexMap.cpp
            src/EdgeIterator.cpp
            src/BredthFirstTraversal.cpp
            src/PreOrderTraversal.cpp
            src/InOrderTraversal.cpp
            src/PostOrderTraversal.cpp
            src/VertexIterator.cpp
            src/Iterator.cpp
            #src/Array.cpp
            #src/Tree.cpp
           )
set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)

add_subdirectory(external/gtest)

target_include_directories(libvertex INTERFACE "${vertex_SOURCE_DIR}/include")

enable_testing()
add_executable(TestVertex
               test/Vertex.cpp 
               test/Traversal.cpp
               test/Iterator.cpp
              #test/Tree.cpp 
              #test/Forest.cpp
              #test/Array.cpp 
               test/main.cpp
              )
target_link_libraries(TestVertex gtest libvertex)
set_target_properties(TestVertex PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestVertex
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestVertex)
