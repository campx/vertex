if(NOT TARGET toolbox)
    configure_file(CMakeLists.txt.in
                   ${CMAKE_BINARY_DIR}/toolbox-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/toolbox-download )
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/toolbox-download )

    add_subdirectory(${CMAKE_BINARY_DIR}/toolbox-src
                     ${CMAKE_BINARY_DIR}/toolbox-build)
endif()
