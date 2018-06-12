cmake_minimum_required(VERSION 2.8)

project(vertex LANGUAGES CXX)

include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

add_library(libvertex
            vertex/Link.cpp                  vertex/Link.h
            vertex/LinkIterator.cpp          vertex/LinkIterator.h
            vertex/Edge.cpp                  vertex/Edge.h
            vertex/Node.cpp                  vertex/Node.h
            vertex/Array.cpp                 vertex/Array.h
            vertex/LinkedList.cpp            vertex/LinkedList.h
            vertex/ManagedContainer.cpp      vertex/ManagedContainer.h
            vertex/BreadthFirstTraversal.cpp vertex/BreadthFirstTraversal.h
            vertex/PreOrderTraversal.cpp     vertex/PreOrderTraversal.h
            vertex/InOrderTraversal.cpp      vertex/InOrderTraversal.h
            vertex/Predicate.cpp             vertex/Predicate.h
            vertex/Traversal.cpp             vertex/Traversal.h
            vertex/PostOrderTraversal.cpp    vertex/PostOrderTraversal.h
            vertex/PathMap.cpp               vertex/PathMap.h)

add_dependencies(libvertex toolbox)
set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)
target_compile_features(libvertex PUBLIC cxx_return_type_deduction)
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	target_compile_options(libvertex PUBLIC -Wall -Werror -Wextra)
endif()

add_subdirectory(external/gtest)
add_subdirectory(external/toolbox)

target_include_directories(libvertex PUBLIC "${vertex_SOURCE_DIR}")

install(TARGETS libvertex 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY vertex DESTINATION include 
        FILES_MATCHING PATTERN "*.h" 
        PATTERN "test/*" EXCLUDE
        PATTERN "test" EXCLUDE)

enable_testing()

SET(GTEST_LIBRARIES gtest)

add_executable(TestVertex
               vertex/test/Link.cpp           vertex/test/Link.h
               vertex/test/Node.cpp           vertex/test/Node.h
               vertex/test/PathMap.cpp
               vertex/test/LinkIterator.cpp
               vertex/test/Traversal.cpp
               vertex/test/Array.cpp
               vertex/test/main.cpp)

add_dependencies(TestVertex googletest toolbox)

if ((${CMAKE_BUILD_TYPE} MATCHES Debug) AND WIN32)
	target_link_libraries(TestVertex gtestd libvertex)
else()
	target_link_libraries(TestVertex gtest libvertex)
endif()

set_target_properties(TestVertex PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestVertex
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestVertex)
