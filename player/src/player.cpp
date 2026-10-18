#include "player.hpp"
#include "song.hpp"
#include <optional>

void Player::addSong(Song song){
    songs.push_back(song);
}

void Player::setSong(int index){
    if(index < 0 || index >= songs.size())
        return;
    currentSongIndex = index;
}

void Player::pauseSong(){
    currentSongPlaying = false;
}

void Player::playSong(){
    currentSongPlaying = true;
}

std::optional<Song> Player::nextSong(){
    if(songs.empty()) return std::nullopt;
    int nextIndex = currentSongIndex + 1;
    if(nextIndex >= songs.size()){
        nextIndex = 0;
    }
    currentSongIndex = nextIndex;
    return songs[currentSongIndex];
}

std::optional<Song> Player::previousSong(){
    if(songs.empty()) return std::nullopt;
    int nextIndex = currentSongIndex - 1;
    if(nextIndex < 0){
        nextIndex = songs.size() - 1;
    }
    currentSongIndex = nextIndex;
    return songs[currentSongIndex];
}

std::optional<Song> Player::getCurrentSong(){
    if(currentSongIndex >= songs.size() || currentSongIndex < 0){
        return std::nullopt;
    }
    return songs[currentSongIndex];
}

const std::vector<Song>& Player::getSongs(){
    return songs;
}

bool Player::isCurrentSongPlaying(){
    return currentSongPlaying;
}

int Player::getCurrentSongsIndex(){
    return currentSongIndex;
}
