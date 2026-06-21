cmake_minimum_required(VERSION 4.2)
project(qualquer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vulkan)
add_subdirectory(optix)
add_subdirectory(renderer)
add_subdirectory(app)
