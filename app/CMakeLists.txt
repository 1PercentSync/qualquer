add_executable(qualquer_app
    src/main.cpp
    src/application.cpp
)

target_include_directories(qualquer_app
    PRIVATE include
)

target_link_libraries(qualquer_app
    PRIVATE qualquer_renderer
)
