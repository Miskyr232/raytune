#include "player-view.hpp"
#include "player.hpp"
#include "raylib.h"
#include "raygui.h"
#include "song.hpp"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

void setTheme(){
    // Text
    GuiSetStyle(DEFAULT, TEXT_SIZE, 27);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);

    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL,
        ColorToInt(RAYWHITE));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED,
        ColorToInt(RAYWHITE));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED,
        ColorToInt(GRAY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_DISABLED,
        ColorToInt(GRAY));

    // Background
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR,
        ColorToInt((Color){20, 20, 20, 255}));

    // Base colors
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL,
        ColorToInt((Color){30, 30, 30, 255}));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED,
        ColorToInt((Color){40, 40, 40, 255}));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED,
        ColorToInt((Color){50, 50, 50, 255}));
    GuiSetStyle(DEFAULT, BASE_COLOR_DISABLED,
        ColorToInt((Color){20, 20, 20, 255}));

    // Borders
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL,
        ColorToInt((Color){60, 60, 60, 255}));
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED,
        ColorToInt((Color){100, 100, 100, 255}));
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED,
        ColorToInt((Color){120, 120, 120, 255}));
    GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED,
        ColorToInt((Color){40, 40, 40, 255}));

    // Inner borders
    GuiSetStyle(DEFAULT, BORDER_WIDTH,
        1);

    GuiSetStyle(DEFAULT, TEXT_PADDING,
        5);
}

void PlayerView::run(){
    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE |
        FLAG_WINDOW_ALWAYS_RUN
    );
    InitWindow(800, 600, "test");
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(30);

    width = GetScreenWidth();
    height = GetScreenHeight();
    int codepoints[256 + 95];

    for (int i = 0; i < 95; i++)
        codepoints[i] = 32 + i;

    for (int i = 0; i < 256; i++)
        codepoints[95 + i] = 0x0400 + i;

    Font JetBrains = LoadFontEx(
        "assets/fonts/JetBrainsMono.ttf",
        27,
        codepoints,
        95 + 256
    );

    GuiSetFont(JetBrains);
    setTheme();

    parseMusicFolder();
    songsNamesString = parseSongsToString();
    songChanged();

    audioPlayer.pause();
    player.pauseSong();

    while (!WindowShouldClose())
    {
        update();
        BeginDrawing();
        ClearBackground((Color){20, 20, 20, 255});
        render();
        EndDrawing();
        postDrawUpdate();
    }
    audioPlayer.unload();
    UnloadFont(JetBrains);
    CloseAudioDevice();
    CloseWindow();
}

Vector2 PlayerView::getPosByPercent(Vector2 percent, Vector2 size){
    return (Vector2){
        (width * percent.x) - (size.x / 2),
        (height * percent.y) - (size.y / 2)
    };
}


void PlayerView::songChanged(){
    auto song = player.getCurrentSong();
    if(song){
        if(audioPlayer.load(*song)){
            audioPlayer.play();
            player.playSong();
            activeSongIndex = player.getCurrentSongsIndex();
        }
    }
}

std::string PlayerView::parseSecsToPretty(int secs){
    int minutes = secs / 60;
    int seconds = secs % 60;

    std::stringstream stream;
    stream << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;

    return stream.str();
}

void PlayerView::renderLabelAtPercent(const char* text, Vector2 percent){
    Font font = GuiGetFont();
    float fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float spacing = GuiGetStyle(DEFAULT, TEXT_SPACING);

    Vector2 titleSize = MeasureTextEx(
        font,
        text,
        fontSize,
        spacing
    );

    float padding = GuiGetStyle(DEFAULT, TEXT_PADDING);

    Vector2 size = {
        titleSize.x + padding * 2,
        titleSize.y + padding * 2
    };

    Vector2 pos = getPosByPercent(
        percent,
        size
    );

    GuiLabel(
        (Rectangle){
            pos.x + sideBarWidth,
            pos.y,
            size.x,
            size.y
        },
        text
    );
}

void PlayerView::render(){
    renderSongInfo();
    renderSongProgress();
    renderControlls();
    renderVolume();
    renderPan();
    renderSongsList();
}

void PlayerView::postDrawUpdate(){
    float dt = GetFrameTime();
    float duration = audioPlayer.getDuration();

    if (duration <= 0.0f)
        return;

    int direction = 0;
    if(IsKeyDown(KEY_LEFT)) direction += -1;
    if(IsKeyDown(KEY_RIGHT)) direction += 1;

    if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)){
        rewindingByKeys = true;
        if(direction != rewindDirection){
            rewindTime = 0.0f;
            rewindDirection = direction;
        }
        float seekSpeed = 0.1f + rewindTime / 5.0f;
        rewindTime += dt;
        if(IsKeyDown(KEY_LEFT)){
            elapsed -= seekSpeed * dt;
        }if(IsKeyDown(KEY_RIGHT)){
            elapsed += seekSpeed * dt;
        }
        elapsed = std::clamp(elapsed, 0.0f, 1.0f);
        if(elapsed >= 1.0f){
            elapsed -= 0.0000001f;
        }
    }else if(rewindingByKeys){
        audioPlayer.seek(elapsed * duration);
        rewindingByKeys = false;
        rewindTime = 0.0f;
    }
}

std::string PlayerView::parseSongsToString(){
    std::stringstream stream;
    const auto& songs = player.getSongs();
    for(int i = 0; i < songs.size(); i++){
        stream << songs[i].title;
        if(i != songs.size() - 1){
            stream << ";";
        }
    }
    return stream.str();
}

void PlayerView::update(){
    float dt = GetFrameTime();
    audioPlayer.update();
    if(IsWindowResized()){
        width = GetScreenWidth();
        height = GetScreenHeight();
    }if(IsKeyPressed(KEY_SPACE)){
        togglePlay();
    }

    float volumeSpeed = 0.5f;
    if(IsKeyDown(KEY_UP)){
        settings.volume = std::clamp(settings.volume + volumeSpeed * dt, 0.0f, 1.0f);
        isMuted = false;
    }if(IsKeyDown(KEY_DOWN)){
        settings.volume = std::clamp(settings.volume - volumeSpeed * dt, 0.0f, 1.0f);
        isMuted = false;
    }
    if(IsKeyPressed(KEY_M)){
        if(isMuted){
            isMuted = false;
            settings.volume = beforeMutedVolume;
        }else{
            isMuted = true;
            beforeMutedVolume = settings.volume;
            settings.volume = 0.0f;
        }
    }
    setVolume(settings.volume * settings.volume);

    float eps = 0.1f;
    float duration = audioPlayer.getDuration();
    if(duration && duration - audioPlayer.getTimePlayed() < eps){
        nextSong();
    }

    if(previousActiveSongIndex != activeSongIndex){
        if(activeSongIndex != -1){
            player.setSong(activeSongIndex);
            songChanged();
        }

        previousActiveSongIndex = activeSongIndex;
    }
}

std::string PlayerView::getWithoutDot(const std::string& ext){
    if(ext.size() <= 0) return "";
    if(ext[0] == '.'){
        return ext.substr(1, ext.size() - 1);
    }
    return ext;
}

void PlayerView::parseMusicFolder(){
    FilePathList songs = LoadDirectoryFiles("music");
    for(unsigned int i = 0; i < songs.count; i++){
        const char* path = songs.paths[i];
        if(!DirectoryExists(path)){
            const char* ext = GetFileExtension(path);
            if(ext != nullptr && (strcmp(ext, ".mp3") == 0 || strcmp(ext, ".wav") == 0 || strcmp(ext, ".ogg") == 0 || strcmp(ext, ".flac") == 0)){

                const char* name = GetFileNameWithoutExt(path);
                Song s;
                s.path = path;
                s.title = name;
                s.extension = std::string(getWithoutDot(ext));
                player.addSong(s);
            }
        }
    }
    UnloadDirectoryFiles(songs);
}


void PlayerView::togglePlay(){
    if(player.isCurrentSongPlaying()){
        player.pauseSong();
        audioPlayer.pause();
    }else{
        player.playSong();
        audioPlayer.resume();
    }
}

void PlayerView::setVolume(float value){
    audioPlayer.setVolume(value);
}

void PlayerView::setPan(float value){
    settings.pan = value;
    audioPlayer.setPan(value);
}

void PlayerView::renderSongInfo(){
    auto s = player.getCurrentSong();
    if(s){
        renderLabelAtPercent((*s).title.c_str(), {0.5f, 0.6f});
        renderLabelAtPercent((*s).artist.c_str(), {0.5f, 0.7f});
        renderLabelAtPercent((*s).extension.c_str(), {0.5f, 0.75f});
    }
}
void PlayerView::renderSongProgress(){
    float duration = audioPlayer.getDuration();
    float position = audioPlayer.getTimePlayed();

    Vector2 size = {200, 10};
    Vector2 pos = getPosByPercent({0.5f, 0.8f}, size);

    Rectangle slider = {
        pos.x + sideBarWidth,
        pos.y,
        size.x,
        size.y
    };

    if (CheckCollisionPointRec(GetMousePosition(), slider) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        draggingSlider = true;
    }

    if (!draggingSlider && !rewindingByKeys && duration > 0.0f)
    {
        elapsed = position / duration;
    }

    GuiSlider(
        slider,
        parseSecsToPretty((int)position).c_str(),
        parseSecsToPretty((int)duration).c_str(),
        &elapsed,
        0.0f,
        1.0f
    );

    if (draggingSlider &&
        IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        draggingSlider = false;

        if (duration > 0.0f)
        {
            audioPlayer.seek(elapsed * duration);
        }
    }
}

void PlayerView::renderControlls(){
    {
    Vector2 size = {40, 40};
    Vector2 pos = getPosByPercent({0.6f, 0.9f}, size);
    if(GuiButton(
        (Rectangle){
            pos.x + sideBarWidth,
            pos.y,
            size.x,
            size.y
        },
        ">|"
    )){
        nextSong();
    }
    }
    {
        Vector2 size = {40, 40};
        Vector2 pos = getPosByPercent({0.5f, 0.9f}, size);
        if(GuiButton(
            (Rectangle){
                pos.x + sideBarWidth,
                pos.y,
                size.x,
                size.y
            },
            player.isCurrentSongPlaying() ? "||" : "|>"
        )){
            togglePlay();
        }
    }
    {
        Vector2 size = {40, 40};
        Vector2 pos = getPosByPercent({0.4f, 0.9f}, size);
        if(GuiButton(
            (Rectangle){
                pos.x + sideBarWidth,
                pos.y,
                size.x,
                size.y
            },
            "|<"
        )){
            previousSong();
        }
    }
}
void PlayerView::renderVolume(){
    Vector2 size = {100, 20};
    Vector2 pos = getPosByPercent({0.8f, 0.1f}, size);

    Rectangle slider = {
        pos.x,
        pos.y,
        size.x,
        size.y
    };

    GuiSlider(
        slider,
        (std::to_string((int)(settings.volume * 100)) + "%").c_str() ,
        "100%",
        &settings.volume,
        0.0f,
        1.0f
    );
}
void PlayerView::renderPan(){
    Vector2 size = {100, 20};
    Vector2 pos = getPosByPercent({0.8f, 0.2f}, size);

    Rectangle slider = {
        pos.x,
        pos.y,
        size.x,
        size.y
    };

    if(!GuiSlider(
        slider,
        "LEFT",
        "RIGHT",
        &settings.pan,
        0.0f,
        1.0f
    )){
        setPan(settings.pan);
    };
}
void PlayerView::renderSongsList(){
    Vector2 size = {200 + sideBarWidth, (float)height};
    Vector2 pos = getPosByPercent({0.0f, 0.5f}, size);

Rectangle list = {
    0,
    pos.y,
    300.0f, // ширина списка
    size.y
};
    GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiListView(
        list,
        songsNamesString.c_str(),
        &scrollIndex,
        &activeSongIndex
    );
}
void PlayerView::renderShuffle(){
    Vector2 size = {30, 30};
    Vector2 pos = getPosByPercent({0.1f, 0.0f}, size);

    Rectangle button = {
        pos.x + size.x / 2 + 110,
        pos.y + size.y / 2,
        size.x,
        size.y
    };
    GuiButton(
        button,
        "shuffle"
    );
}

void PlayerView::nextSong(){
    auto song = player.nextSong();
    if(song){
        playSong(*song);
        activeSongIndex = player.getCurrentSongsIndex();
    }
}

void PlayerView::previousSong(){
    auto song = player.previousSong();
    if(song){
        playSong(*song);
        activeSongIndex = player.getCurrentSongsIndex();
    }
}

void PlayerView::playSong(const Song& song){
    if(audioPlayer.load(song)){
        audioPlayer.play();
        player.playSong();
    }
}
