cmake_minimum_required(VERSION 2.8)

project(vertex LANGUAGES CXX)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    message(STATUS "Using MSVC")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # using Clang
        message(STATUS "Using Clang")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU) # using GCC
        message(STATUS "Using GNU")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
    endif()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #create compilation database for clang

add_library(libvertex
    include/vertex/Link.h                   src/Link.cpp
    include/vertex/LinkIterator.h           src/LinkIterator.cpp
    include/vertex/Node.h                   src/Node.cpp
    include/vertex/Array.h                  src/Array.cpp
    include/vertex/ManagedContainer.h       src/ManagedContainer.cpp
    include/vertex/EdgeIterator.h           src/EdgeIterator.cpp
    include/vertex/BreadthFirstTraversal.h  src/BreadthFirstTraversal.cpp
    include/vertex/PreOrderTraversal.h      src/PreOrderTraversal.cpp
    include/vertex/InOrderTraversal.h       src/InOrderTraversal.cpp
	include/vertex/Predicate.h				src/Predicate.cpp
    include/vertex/Traversal.h              src/Traversal.cpp
    include/vertex/PostOrderTraversal.h     src/PostOrderTraversal.cpp
    include/vertex/NodeIterator.h           src/NodeIterator.cpp
    include/vertex/Iterator.h               src/Iterator.cpp 
	include/vertex/Edge.h					src/Edge.cpp)

set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)
target_link_libraries(libvertex libtoolbox)

add_subdirectory(external/gtest)
add_subdirectory(external/toolbox)

target_include_directories(libvertex PUBLIC "${vertex_SOURCE_DIR}/include")

enable_testing()
add_executable(TestVertex
    test/Link.h     test/Link.cpp
                    test/LinkIterator.cpp
    test/Node.h     test/Node.cpp
                    test/Traversal.cpp
                    test/Array.cpp
                    test/Iterator.cpp
                    test/main.cpp)

target_link_libraries(TestVertex gtest libvertex libtoolbox)
set_target_properties(TestVertex PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestVertex
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestVertex)
