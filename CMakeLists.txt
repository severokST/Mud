cmake_minimum_required(VERSION 3.16)
project(MudServer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create executable
add_executable(mud_server ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(mud_server 
    Threads::Threads
)

# Set output directory
set_target_properties(mud_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Enable debug symbols for debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(mud_server PRIVATE -g)
endif()
