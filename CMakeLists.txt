cmake_minimum_required(VERSION 3.10)

project(vertex VERSION 1.0 LANGUAGES CXX)
include(CTest)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

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

set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)
target_include_directories(libvertex PUBLIC
        $<BUILD_INTERFACE:${vertex_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

if (MSVC)
    target_compile_options(libvertex PRIVATE /W4 /WX /MP)
else ()
    target_compile_options(libvertex PRIVATE -Wall -Wextra -pedantic -Werror)
    target_link_libraries(libvertex PUBLIC stdc++fs)
endif()

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DNDEBUG)
endif ()

install(TARGETS libvertex
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY vertex DESTINATION include 
        FILES_MATCHING PATTERN "*.h" 
        PATTERN "test/*" EXCLUDE
        PATTERN "test" EXCLUDE)

if (BUILD_TESTING)
    find_package(GTest MODULE REQUIRED)
    add_executable(vertex_test
            vertex/test/Link.cpp vertex/test/Link.h
            vertex/test/Node.cpp vertex/test/Node.h
            vertex/test/PathMap.cpp
            vertex/test/LinkIterator.cpp
            vertex/test/Traversal.cpp
            vertex/test/Array.cpp
            vertex/test/main.cpp)
    target_link_libraries(vertex_test PRIVATE libvertex GTest::GTest GTest::Main)
    gtest_discover_tests(vertex_test)
    if (MSVC)
        target_compile_options(vertex_test PRIVATE /W4 /WX /MP)
    else ()
        target_compile_options(vertex_test PRIVATE -Wall -Wextra -pedantic -Werror)
    endif ()
endif ()
