#include "player.hpp"
#include <raylib.h>
#include <string>
#include "audio-player.hpp"

enum Mode{
    LISTEN,
    SELECT_SONG
};

class PlayerView{
    AudioPlayer audioPlayer;
    Mode mode = LISTEN;
    int width;
    int height;

    Player player;
    float elapsed = 0.0f;
    bool draggingSlider = false;
    bool rewindingByKeys = false;
    float rewindTime = 0.0f;
    int rewindDirection = 0; // -1 - left, 0 - stop, 1 - right

    bool isMuted = false;
    float beforeMutedVolume = 0.0f;

    PlayerSettings settings;

    int scrollIndex = 0;
    int previousActiveSongIndex = -1;
    int activeSongIndex = -1;


    float sideBarWidth = 150;

    Texture2D mothership = LoadTexture("assets/icons/download.jpg");

    std::string songsNamesString;

    void update();
    void postDrawUpdate();
    void render();

    void songChanged();

    void togglePlay();
    void setVolume(float value);
    void setPan(float value);

    void parseMusicFolder();
    std::string getWithoutDot(const std::string& ext);
    std::string parseSecsToPretty(int secs);
    std::string parseSongsToString();
    Vector2 getPosByPercent(Vector2 percent, Vector2 size);

    void renderSongInfo();
    void renderSongProgress();
    void renderControlls();
    void renderVolume();
    void renderPan();
    void renderSongsList();
    void renderShuffle();
    void renderLabelAtPercent(const char* test, Vector2 percent);
    void nextSong();
    void previousSong();
    void playSong(const Song& song);
public:
    void run();
};
