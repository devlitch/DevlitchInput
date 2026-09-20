#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Shortcut.h"

class Checker {
public:
    bool init();
private:
    uint32_t findAppID(std::string target, std::vector<Shortcut> shortcuts);
    uint64_t AppIDToRunGameID(uint32_t appid);
};