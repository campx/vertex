cmake_minimum_required(VERSION 2.8)

project(merkle LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
include_directories(${merkle_SOURCE_DIR}/include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #create compilation database for clang

add_library(libmerkle
            src/Node.cpp
            src/Forest.cpp)
set_target_properties(libmerkle PROPERTIES OUTPUT_NAME merkle)

add_subdirectory(external/gtest)

enable_testing()
add_executable(TestMerkle test/TestNode.cpp test/TestForest.cpp test/main.cpp)
target_link_libraries(TestMerkle gtest libmerkle)
set_target_properties(TestMerkle PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestMerkle
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestMerkle)
