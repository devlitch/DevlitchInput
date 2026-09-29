#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Shortcut.h"

class Checker {
public:
    bool init();
private:
    uint32_t findAppID(const std::string& target, const std::vector<Shortcut>& shortcuts);
    uint64_t AppIDToRunGameID(uint32_t appid);
};