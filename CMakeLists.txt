cmake_minimum_required(VERSION 2.8)

project(vertex LANGUAGES CXX)

IF (NOT MSVC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra")
  IF (MINGW)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu++14")
  ELSE()
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++14")
  ENDIF(MINGW)
ENDIF(NOT MSVC)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #create compilation database for clang

add_library(libvertex
    include/vertex/Link.h                   src/Link.cpp
    include/vertex/Vertex.h                 src/Vertex.cpp
    include/vertex/ManagedVertexMap.h       src/ManagedVertexMap.cpp
    include/vertex/EdgeIterator.h           src/EdgeIterator.cpp
    include/vertex/BreadthFirstTraversal.h  src/BreadthFirstTraversal.cpp
    include/vertex/PreOrderTraversal.h      src/PreOrderTraversal.cpp
    include/vertex/InOrderTraversal.h       src/InOrderTraversal.cpp
	include/vertex/Predicate.h				src/Predicate.cpp
    include/vertex/Traversal.h              src/Traversal.cpp
    include/vertex/PostOrderTraversal.h     src/PostOrderTraversal.cpp
    include/vertex/VertexIterator.h         src/VertexIterator.cpp
    include/vertex/Iterator.h               src/Iterator.cpp 
	include/vertex/Edge.h					src/Edge.cpp)

set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)

add_subdirectory(external/gtest)

target_include_directories(libvertex PUBLIC "${vertex_SOURCE_DIR}/include")

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
