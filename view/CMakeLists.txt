add_executable(view
    src/main.cpp
    src/player-view.cpp
    src/audio-player.cpp
)

target_include_directories(view PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(view PRIVATE
    player
    raylib
    raygui
)
