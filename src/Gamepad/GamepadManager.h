#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "../InputState.h"
#include "../Input/ControllerManager.h"
#include "../Bridge/ViGEmManager.h"

struct Controller
{
    SDL_Gamepad* pad = nullptr;
    SDL_JoystickID id;
    bool rumble;

    InputState state;
};

class ViGEmManager;

class GamepadManager
{
public:
    void addController(SDL_JoystickID id);
    void removeController(SDL_JoystickID id);
    bool Initialize();
    void Update();
    Controller* find(SDL_JoystickID id);

private:
    std::vector<Controller> controllers;
    float normalize(int value);
};