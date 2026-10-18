#pragma once

#include "song.hpp"
#include <vector>
#include <optional>

struct PlayerSettings{
    float volume = 0.5f;
    float pitch = 1.0f;
    float pan = 0.5f;
    bool looping = false;
};

class Player {
    std::vector<Song> songs;
    int currentSongIndex = 0;
    bool currentSongPlaying = false;
public:
    void addSong(Song song);
    void setSong(int index);
    void pauseSong();
    void playSong();
    std::optional<Song> nextSong();
    std::optional<Song> previousSong();
    bool isCurrentSongPlaying();
    const std::vector<Song>& getSongs();
    std::optional<Song> getCurrentSong();
    int getCurrentSongsIndex();
};
