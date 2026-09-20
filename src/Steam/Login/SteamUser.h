#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <SDL3_image/SDL_image.h>
#include "../../GUI/Renderer/Texture.h"

struct SteamUser
{
    uint64_t id;
    std::string name;
    Texture* avatar;
    std::string avatarUrl;
    bool local;
};