#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "../InputState.h"

struct Controller {
    SDL_Gamepad* pad = nullptr;
    SDL_JoystickID id;
    bool rumble;

    InputState state;
};

class GamepadManager {
public:
    bool addController(SDL_JoystickID id);
    bool removeController(SDL_JoystickID id);
    bool Initialize();
    void Update();
    Controller* find(SDL_JoystickID id);
    void ProcessEvent(const SDL_Event& e);
    void Start();
    bool TestRumble(SDL_JoystickID id, Uint16 smallMotor, Uint16 largeMotor, Uint32 duration);
private:
    std::vector<Controller> controllers;
    float normalize(int value);
};

inline GamepadManager gamepadManager;