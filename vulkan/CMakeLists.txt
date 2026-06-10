add_library(qualquer_vulkan STATIC)

target_sources(qualquer_vulkan PRIVATE
    src/placeholder.cpp
)

target_include_directories(qualquer_vulkan
    PUBLIC include
)
