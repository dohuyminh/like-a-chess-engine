add_library(builder STATIC app_builder.cpp app_builder.h)

target_include_directories(builder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)