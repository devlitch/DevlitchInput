#include "ControllerManager.h"

ControllerManager controllerManager;
extern GamepadManager gamepadManager;

ControllerInfo* ControllerManager::find(SDL_JoystickID id) {
    for (auto& c : controllersInfo) {
        if (c.id == id) return &c;
    }
    return nullptr;
}

void ControllerManager::toggleControllerState(SDL_JoystickID id) {
    ControllerInfo* c = find(id);
    if (!c) return;

    c->connected = !c->connected;
    if (c->connected) {
        gamepadManager.addController(c->id);
    } else {
        gamepadManager.removeController(c->id);
    }
}

void ControllerManager::Refresh() {
    controllersInfo.clear();

    int count = 0;
    SDL_JoystickID* ids = SDL_GetJoysticks(&count);

    for (int i = 0; i < count; i++) {
        SDL_JoystickID id = ids[i];
        const char* path = SDL_GetJoystickPathForID(id);
        if (path && std::strncmp(path, "XInput", 6) != 0) continue;
        Controller* cGamepad = gamepadManager.find(id);
        SDL_Gamepad* pad = SDL_OpenGamepad(id);
        if (!pad) continue;
        Uint64 steamHandle = SDL_GetGamepadSteamHandle(pad);
        if (steamHandle == 0) continue;

        SDL_CloseGamepad(pad);
        ControllerInfo c;
        c.id = id;
        c.name = std::string(SDL_GetJoystickNameForID(id)) + " ( #"+std::to_string(id) +" )";
        c.connected = cGamepad?true:false;

        if (c.name == "") c.name = "Unknown";

        controllersInfo.push_back(c);
    }

    SDL_free(ids);
}

const std::vector<ControllerInfo>& ControllerManager::GetControllers() const {
    return controllersInfo;
}