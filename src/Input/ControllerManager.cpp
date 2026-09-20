#include "ControllerManager.h"

#include "../Bridge/ViGEmManager.h"
#include "../Gamepad/GamepadManager.h"

ControllerInfo* ControllerManager::find(SDL_JoystickID id) {
    for (auto& c : controllersInfo) {
        if (c.id == id) return &c;
    }
    return nullptr;
}

bool ControllerManager::setControllerState(SDL_JoystickID id, bool type) {
    ControllerInfo* c = find(id);
    if (!c) return false;

    c->connected = type;
    return type ? gamepadManager.addController(c->id) : gamepadManager.removeController(c->id);
}

bool ControllerManager::Rumble(SDL_JoystickID id, Uint16 smallMotor, Uint16 largeMotor, Uint32 duration) {
    return gamepadManager.TestRumble(id, smallMotor, largeMotor, duration);
}

void ControllerManager::Refresh() {
    controllersInfo.clear();

    int count = 0;
    SDL_JoystickID* ids = SDL_GetJoysticks(&count);

    for (int i = 0; i < count; i++) {
        SDL_JoystickID id = ids[i];

        Uint16 vid = SDL_GetJoystickVendorForID(id);
        Uint16 pid = SDL_GetJoystickProductForID(id);
        const char* path = SDL_GetJoystickPathForID(id);
        if (path && std::strncmp(path, "XInput", 6) != 0) continue;
        Controller* cGamepad = gamepadManager.find(id);

        ControllerInfo c;
        c.id = id;
        c.name = std::string(SDL_GetJoystickNameForID(id)) + " ( #" + std::to_string(id) + " )";
        c.connected = cGamepad ? true : false;

        if (bridge.isOurDevice(vid, pid)) continue;

        if (c.name == "") c.name = "Unknown";

        controllersInfo.push_back(c);
    }

    SDL_free(ids);
}

const std::vector<ControllerInfo>& ControllerManager::GetControllers() const {
    return controllersInfo;
}