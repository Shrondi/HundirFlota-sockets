cmake_minimum_required(VERSION 3.16)

# The top-level CMakeLists.txt file must
project(HundirFlot LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_BUILD_TYPE Debug)

# Enables testing/add_test() for this directory and below
# enable_testing()

add_subdirectory(src)
