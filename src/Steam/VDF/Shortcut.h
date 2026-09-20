#pragma once
#include <string>
#include <vector>

struct Shortcut {
    uint32_t appid = 0;

    std::string AppName;
    std::string Exe;
    std::string StartDir;
    std::string Icon;
    std::string ShortcutPath;
    std::string LaunchOptions;

    bool IsHidden = 0;
    bool AllowDesktopConfig = 0;
    bool AllowOverlay = 0;
    bool OpenVR = 0;
    bool Devkit = 0;

    std::string DevkitGameID;

    uint32_t DevkitOverrideAppID = 0;
    uint32_t LastPlayTime = 0;

    std::string FlatpakAppID;
    std::string SortAs;

    std::vector<std::string> Tags;
};