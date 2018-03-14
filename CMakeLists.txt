cmake_minimum_required(VERSION 2.8)

project(vertex LANGUAGES CXX)

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")
endif()

SET(GTEST_LIBRARIES gtest)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        SET(GTEST_LIBRARIES gtestd)
    endif()
endif()

include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

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
    include/vertex/VertexIterator.h         src/VertexIterator.cpp
    include/vertex/Iterator.h               src/Iterator.cpp 
	include/vertex/Edge.h					src/Edge.cpp)

add_dependencies(libvertex toolbox)
set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)
target_compile_features(libvertex PUBLIC cxx_return_type_deduction)

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

add_dependencies(TestVertex googletest toolbox)
target_link_libraries(TestVertex ${GTEST_LIBRARIES} libvertex)
set_target_properties(TestVertex PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestVertex
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestVertex)
