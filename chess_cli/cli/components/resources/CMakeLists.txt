add_library(resources STATIC
    resources.cpp
    resources.h
    pcb.cpp
    pcb.h
)

target_include_directories(resources
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
