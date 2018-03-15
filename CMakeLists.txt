cmake_minimum_required(VERSION 2.8)

project(vertex LANGUAGES CXX)

include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

add_library(libvertex
            src/Link.cpp                  include/vertex/Link.h
            src/LinkIterator.cpp          include/vertex/LinkIterator.h
            src/Node.cpp                  include/vertex/Node.h
            src/Array.cpp                 include/vertex/Array.h
            src/ManagedContainer.cpp      include/vertex/ManagedContainer.h
            src/EdgeIterator.cpp          include/vertex/EdgeIterator.h
            src/BreadthFirstTraversal.cpp include/vertex/BreadthFirstTraversal.h
            src/PreOrderTraversal.cpp     include/vertex/PreOrderTraversal.h
            src/InOrderTraversal.cpp      include/vertex/InOrderTraversal.h
            src/Predicate.cpp             include/vertex/Predicate.h
            src/Traversal.cpp             include/vertex/Traversal.h
            src/PostOrderTraversal.cpp    include/vertex/PostOrderTraversal.h
            src/VertexIterator.cpp        include/vertex/VertexIterator.h
            src/Iterator.cpp              include/vertex/Iterator.h
            src/Edge.cpp)                 include/vertex/Edge.h)

add_dependencies(libvertex toolbox)
set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)
target_compile_features(libvertex PUBLIC cxx_return_type_deduction)
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	target_compile_options(libvertex PUBLIC -Wall -Werror -Wextra)
endif()

add_subdirectory(external/gtest)
add_subdirectory(external/toolbox)

target_include_directories(libvertex PUBLIC "${vertex_SOURCE_DIR}/include")

enable_testing()

SET(GTEST_LIBRARIES gtest)

add_executable(TestVertex
               test/Link.cpp           test/Link.h
               test/Node.cpp           test/Node.h
               test/LinkIterator.cpp
               test/Traversal.cpp
               test/Array.cpp
               test/Iterator.cpp
               test/main.cpp)

add_dependencies(TestVertex googletest toolbox)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug AND ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	target_link_libraries(TestVertex gtestd libvertex)
else()
	target_link_libraries(TestVertex gtest libvertex)
endif()

set_target_properties(TestVertex PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestVertex
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestVertex)
