# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(nsx)

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the C++ compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -w")

# Find all .cpp source files in the src directory
file(GLOB SRCS "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Add the executable target
add_executable(nsx ${SRCS})

# Configure clip library
set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples")
set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests")
add_subdirectory(clip)

# Link the required libraries
target_link_libraries(nsx ncurses panel clip) 

target_include_directories(nsx PRIVATE ${CMAKE_SOURCE_DIR}/clip)