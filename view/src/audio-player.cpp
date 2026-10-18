#include "audio-player.hpp"
#include <raylib.h>

AudioPlayer::~AudioPlayer(){
    unload();
}

bool AudioPlayer::load(const Song& song){
    unload();
    currentSong = LoadMusicStream(song.path.c_str());
    if(!IsMusicValid(currentSong)){
        currentSong = {0};
        return false;
    }
    loaded = true;
    return true;
}

void AudioPlayer::unload(){
    if(!loaded) return;
    UnloadMusicStream(currentSong);
    currentSong = { 0 };
    loaded = false;
}

void AudioPlayer::play(){
    if(!loaded){
        TraceLog(LOG_WARNING, "AudioPlayer: no song loaded");
        return;
    }
    SeekMusicStream(currentSong, 0.0f);
    PlayMusicStream(currentSong);
}

void AudioPlayer::pause(){
    if(!loaded) return;
    PauseMusicStream(currentSong);
}

void AudioPlayer::resume(){
    if(!loaded) return;
    ResumeMusicStream(currentSong);
}

void AudioPlayer::update(){
    if(!loaded) return;
    UpdateMusicStream(currentSong);
}

void AudioPlayer::setVolume(float value){
    if(!loaded) return;
    SetMusicVolume(currentSong, value);
}

void AudioPlayer::setPan(float value){
    if(!loaded) return;
    SetMusicPan(currentSong, value);
}

float AudioPlayer::getDuration(){
    if(!loaded) return 0.0f;
    return GetMusicTimeLength(currentSong);
}

void AudioPlayer::seek(float seconds){
    if(!loaded) return;
    SeekMusicStream(currentSong, seconds);
}

float AudioPlayer::getTimePlayed(){
    if(!loaded) return 0.0f;
    return GetMusicTimePlayed(currentSong);
}
