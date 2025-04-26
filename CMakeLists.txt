cmake_minimum_required(VERSION 3.10)
project(countdracula)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find DPP library
find_package(dpp REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Add executable
add_executable(countdracula
    main.cpp
    modules/greetings_module.cpp
    modules/chess/chess_module.cpp
)

# Link libraries
target_link_libraries(countdracula dpp)

# Add filesystem library for older GCC versions if needed
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(countdracula stdc++fs)
endif()
