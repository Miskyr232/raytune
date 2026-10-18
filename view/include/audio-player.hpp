#include <raylib.h>
#include "song.hpp"

class AudioPlayer{
    Music currentSong = { 0 };
    bool loaded = false;
public:
    ~AudioPlayer();
    bool load(const Song& song);
    void play();
    void pause();
    void resume();
    void update();
    void unload();
    bool isFinished() const;
    void setVolume(float value);
    void setPan(float value);
    float getDuration(); //in seconds
    float getTimePlayed();
    void seek(float seconds);
};
