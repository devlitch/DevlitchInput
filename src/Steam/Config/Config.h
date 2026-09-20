#pragma once

#include <string>

class Config {
public:

    bool LoadConfig();
    bool SaveConfig();
    bool ResetConfig();

public:
    std::string User = "";
    std::string accountId = "";
    std::string SteamFolder = "";
    bool SteamInputDisabled = false;

private:
};

inline Config cfg;