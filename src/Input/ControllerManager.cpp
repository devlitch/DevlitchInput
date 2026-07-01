#include "ControllerManager.h"
#include "../Tray/Tray.h"
#include <stdexcept>

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
std::wstring wstring(const char* s) {
    if (!s) return {};

    int n = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        s, -1,
        nullptr, 0
    );

    if (n <= 0) throw std::runtime_error("Invalid UTF-8");

    std::wstring w(n, L'\0');

    MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        s, -1,
        w.data(),
        n
    );

    w.pop_back();
    return w;
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

        ControllerInfo c;
        c.id = id;
        c.name = wstring(SDL_GetJoystickNameForID(id)) + L" ( #" + std::to_wstring(id) + L" )";
        c.connected = cGamepad?true:false;

        if (c.name == L"") c.name = L"Unknown";

        controllersInfo.push_back(c);
        menuDirty = true;
    }

    SDL_free(ids);
}

const std::vector<ControllerInfo>& ControllerManager::GetControllers() const {
    return controllersInfo;
}