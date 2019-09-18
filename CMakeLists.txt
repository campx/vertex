cmake_minimum_required(VERSION 3.10)

project(vertex VERSION 1.0 LANGUAGES CXX)
include(CTest)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_library(libvertex
        vertex/link.cpp
        vertex/link.h
        vertex/link_iterator.cpp
        vertex/link_iterator.h
        vertex/edge.cpp
        vertex/edge.h
        vertex/node.cpp
        vertex/node.h
        vertex/array.cpp
        vertex/array.h
        vertex/linked_list.cpp
        vertex/linked_list.h
        vertex/managed_container.cpp
        vertex/managed_container.h
        vertex/breadth_first_traversal.cpp
        vertex/breadth_first_traversal.h
        vertex/pre_order_traversal.cpp
        vertex/pre_order_traversal.h
        vertex/in_order_traversal.cpp
        vertex/in_order_traversal.h
        vertex/predicate.cpp
        vertex/predicate.h
        vertex/traversal.cpp
        vertex/traversal.h
        vertex/post_order_traversal.cpp
        vertex/post_order_traversal.h
        vertex/path_map.cpp
        vertex/path_map.h
        vertex/iterator.cpp
        vertex/iterator_recorder.cpp
        vertex/iterator_transformer.cpp
        vertex/sequence_predicate.cpp)

set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)
target_include_directories(libvertex PUBLIC
        $<BUILD_INTERFACE:${vertex_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

if (MSVC)
    target_compile_options(libvertex PRIVATE /W4 /WX /MP)
else ()
    target_compile_options(libvertex PRIVATE -Wall -Wextra -pedantic -Werror)
    target_link_libraries(libvertex PUBLIC stdc++fs)
endif ()

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
            vertex/test/link.cpp
            vertex/test/link.h
            vertex/test/node.cpp
            vertex/test/node.h
            vertex/test/path_map.cpp
            vertex/test/link_iterator.cpp
            vertex/test/traversal.cpp
            vertex/test/array.cpp
            vertex/test/main.cpp)
    target_link_libraries(vertex_test PRIVATE libvertex GTest::GTest GTest::Main)
    gtest_discover_tests(vertex_test)
    if (MSVC)
        target_compile_options(vertex_test PRIVATE /W4 /WX /MP)
    else ()
        target_compile_options(vertex_test PRIVATE -Wall -Wextra -pedantic -Werror)
    endif ()
endif ()
