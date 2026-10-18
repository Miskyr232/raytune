#pragma once
#include <string>

struct Song{
    std::string path;
    std::string title = "Unknown title";
    std::string artist = "Unknown artist";
    std::string extension = "";
};
