#pragma once

#include <vector>
#include <string>
#include <SDL3/SDL.h>

struct ControllerInfo {
    SDL_JoystickID id;
    std::string name;
    bool connected = false;
    bool rumble = false;
};

class ControllerManager {
public:
    bool setControllerState(SDL_JoystickID id, bool type);
    bool Rumble(SDL_JoystickID id, Uint16 smallMotor, Uint16 largeMotor, Uint32 duration);
    int GetRumbleState(SDL_JoystickID id);
    bool SwitchRumble(SDL_JoystickID id, bool enabled);
    void Refresh();

    const std::vector<ControllerInfo>& GetControllers() const;

private:
    std::vector<ControllerInfo> controllersInfo;
    ControllerInfo* find(SDL_JoystickID id);
};

inline ControllerManager controllerManager;