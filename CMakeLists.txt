cmake_minimum_required(VERSION 2.8)

project(vertex LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
include_directories(${vertex_SOURCE_DIR}/include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #create compilation database for clang

add_library(libvertex
            src/Vertex.cpp
            src/VertexStore.cpp
            src/EdgeIterator.cpp
            src/BredthFirstTraversal.cpp
            src/EdgeTargetIterator.cpp
           #src/Tree.cpp
           #src/Forest.cpp
           #src/Iterator.cpp
           #src/Array.cpp
           )
set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)

add_subdirectory(external/gtest)

target_include_directories(libvertex INTERFACE "${vertex_SOURCE_DIR}/include")

enable_testing()
add_executable(TestVertex
               test/TestVertex.cpp 
               test/TestBredthFirstTraversal.cpp
               test/TestEdgeTargetIterator.cpp
              #test/TestTree.cpp 
              #test/TestForest.cpp
              #test/TestArray.cpp 
               test/main.cpp
              )
target_link_libraries(TestVertex gtest libvertex)
set_target_properties(TestVertex PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestVertex
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestVertex)
