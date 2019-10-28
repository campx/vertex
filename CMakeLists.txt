cmake_minimum_required(VERSION 3.10)

project(vertex VERSION 1.0 LANGUAGES CXX)

option(BUILD_TESTING "Build unit tests" ON)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

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
        vertex/iterator_recorder.cpp
        vertex/path.cpp
        vertex/path.h)

set_target_properties(libvertex PROPERTIES OUTPUT_NAME vertex)
target_include_directories(libvertex PUBLIC
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
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

if (BUILD_TESTING)
    enable_testing()
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

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${vertex_BINARY_DIR}/vertexConfigVersion.cmake"
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(TARGETS libvertex
        EXPORT vertexTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY vertex DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "test/*" EXCLUDE
        PATTERN "test" EXCLUDE)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        "${vertex_SOURCE_DIR}/cmake/vertexConfig.cmake"
        "${vertex_BINARY_DIR}/vertexConfig.cmake"
        INSTALL_DESTINATION share/cmake/vertex
)

install(EXPORT vertexTargets DESTINATION share/cmake/vertex)
install(FILES "${vertex_BINARY_DIR}/vertexConfigVersion.cmake"
        "${vertex_BINARY_DIR}/vertexConfig.cmake"
        DESTINATION share/cmake/vertex)


