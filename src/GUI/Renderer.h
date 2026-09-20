#pragma once

#include "Renderer/Texture.h"

#include <SDL3/SDL.h>
#include <string>
#include <vector>


class Renderer {
public:
    bool Init(SDL_GPUDevice* device);

    Texture* LoadTexture(const std::string& file);
    Texture* LoadTextureFromMemory(const void* data, size_t size);

    void Destroy();

private:
    SDL_GPUDevice* device = nullptr;
};