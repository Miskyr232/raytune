add_library(player STATIC
    src/player.cpp
)

target_include_directories(player PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
