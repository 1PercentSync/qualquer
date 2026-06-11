add_library(qualquer_vulkan STATIC)

target_sources(qualquer_vulkan PRIVATE
    src/placeholder.cpp
)

target_include_directories(qualquer_vulkan
    PUBLIC include
)

find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(qualquer_vulkan
    PUBLIC spdlog::spdlog
    PUBLIC glfw
)
