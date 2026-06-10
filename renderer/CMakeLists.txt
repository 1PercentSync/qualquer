add_library(qualquer_renderer STATIC)

target_sources(qualquer_renderer PRIVATE
    src/placeholder.cpp
)

target_include_directories(qualquer_renderer
    PUBLIC include
)

target_link_libraries(qualquer_renderer
    PUBLIC qualquer_vulkan
)
