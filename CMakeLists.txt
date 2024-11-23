cmake_minimum_required(VERSION 3.3)

project(serial_comm)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add executables for server and client
add_executable(server server.c)
add_executable(client client.c)

# Include the current directory for header files
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set build output directory to the current directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Enable verbose build output
set(CMAKE_VERBOSE_MAKEFILE ON)

# Custom clean target
add_custom_target(cleanall
    COMMAND ${CMAKE_COMMAND} -E remove
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake_install.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/Makefile
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/client
    COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles
    COMMENT "Deleting all files and folders created by CMake and make..."
)
