cmake_minimum_required(VERSION 2.8)
project(toolbox LANGUAGES CXX)
include(ExternalProject)
 
ExternalProject_Add (
    toolbox
    GIT_REPOSITORY      git@bitbucket.org:jabrooker/toolbox.git
    #GIT_REPOSITORY      git@localhost:campx/toolbox.git
    GIT_TAG             master
    INSTALL_DIR         ${CMAKE_BINARY_DIR}
    CMAKE_ARGS          -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
    PREFIX              ${CMAKE_CURRENT_BINARY_DIR}
    UPDATE_COMMAND      ""
)
