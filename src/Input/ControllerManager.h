#pragma once

#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include "../Gamepad/GamepadManager.h"

struct ControllerInfo
{
    SDL_JoystickID id;
    std::wstring name;
    bool connected = false;
};

class ControllerManager
{
public:
    void toggleControllerState(SDL_JoystickID id);
    void Refresh();

    const std::vector<ControllerInfo>& GetControllers() const;

private:
    std::vector<ControllerInfo> controllersInfo;
    ControllerInfo* find(SDL_JoystickID id);
};