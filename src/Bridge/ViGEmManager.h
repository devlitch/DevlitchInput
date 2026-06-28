#pragma once

#include <Windows.h>
#include <ViGEm/Client.h>
#include <vector>
#include <unordered_map>
#include <SDL3/SDL_gamepad.h>

#include "../InputState.h"

class ViGEmManager
{
public:
    bool Initialize();
    void Shutdown();

    void EnsureController(SDL_JoystickID id);
    void RemoveController(SDL_JoystickID id);

    void Update(SDL_JoystickID id, const InputState& state);

private:
    PVIGEM_CLIENT client = nullptr;

    std::unordered_map<SDL_JoystickID, PVIGEM_TARGET> pads;
};