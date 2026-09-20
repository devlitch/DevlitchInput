#include "GamepadManager.h"

#include "../IPC/TCPServer.h"
#include "../Input/ControllerManager.h"
#include "../Bridge/ViGEmManager.h"

#include <format>

bool GamepadManager::Initialize() {
    SDL_SetHint("SDL_HINT_ENABLE_STEAM_SCREEN_KEYBOARD", "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput - SDL", "SDL init failed", NULL);
        return false;
    }
    SDL_Environment* env = SDL_GetEnvironment();

    if (!SDL_GetEnvironmentVariable(env, "SteamClientLaunch")) return 0;
    controllerManager.Refresh();
    return true;
}

void GamepadManager::Start() {
    while (server.IsRunning()) {
        SDL_PumpEvents();
        SDL_UpdateJoysticks();

        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            gamepadManager.ProcessEvent(e);
        }

        gamepadManager.Update();

        SDL_Delay(5);
    }
}

bool GamepadManager::addController(SDL_JoystickID id) {
    Controller* cId = find(id);
    if (cId) return false;

    SDL_Gamepad* pad = SDL_OpenGamepad(id);
    if (!pad) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput - SDL", std::format("OpenGamepad({}) failed: {}", id, SDL_GetError()).c_str(), nullptr);
        return false;
    }

    bool rumble = SDL_GetBooleanProperty(SDL_GetGamepadProperties(pad), SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false);

    Controller c;
    c.pad = pad;
    c.id = id;
    c.rumble = rumble;
    if (!bridge.EnsureController(id, pad, rumble)) return false;
    controllers.push_back(c);
    TestRumble(id, 2500, 2500, 50);
    SDL_Delay(50);
    TestRumble(id, 2500, 2500, 50);
    return true;
}

bool GamepadManager::removeController(SDL_JoystickID id) {
    Controller* cId = find(id);
    if (!cId) return false;
    TestRumble(id, 2500, 2500, 50);
    SDL_Delay(5);
    bridge.RemoveController(id);

    if (cId->pad) {
        SDL_CloseGamepad(cId->pad);
        cId->pad = nullptr;
    }

    controllers.erase(
        std::remove_if(controllers.begin(), controllers.end(), [&](Controller& c) {
            return c.id == id;
        }), controllers.end()
    );

    return true;
}

Controller* GamepadManager::find(SDL_JoystickID id) {
    for (auto& c : controllers) {
        if (c.id == id) return &c;
    }
    return nullptr;
}

bool GamepadManager::TestRumble(SDL_JoystickID id, Uint16 smallMotor, Uint16 largeMotor, Uint32 duration) {
    Controller* controller = find(id);
    if (!controller) {
        SDL_Joystick* joystick = SDL_OpenJoystick(id);
        if (!joystick) return false;

        bool result = SDL_RumbleJoystick(joystick, smallMotor, largeMotor, duration);
        if (result) {
            std::thread([joystick, duration]() {
                SDL_Delay(duration);
                SDL_CloseJoystick(joystick);
            }).detach();
        } else {
            SDL_CloseJoystick(joystick);
        }
        return result;
    }

    if (!controller->pad) return false;
    if (!controller->rumble) return false;

    return SDL_RumbleGamepad(controller->pad, smallMotor, largeMotor, duration);
}


float GamepadManager::normalize(int value) {
    return value / 32767.0f;
}

void GamepadManager::ProcessEvent(const SDL_Event& e) {
    switch (e.type) {
    case SDL_EVENT_QUIT:
    {
        server.func.Stop();
        return;
    }
    case SDL_EVENT_JOYSTICK_ADDED:
    {
        server.RefreshControllerList();
        break;
    }
    case SDL_EVENT_JOYSTICK_REMOVED:
    {
        SDL_JoystickID id = e.jdevice.which;
        removeController(id);
        server.RefreshControllerList();
        break;
    }
    default:
        break;
    }
}

void GamepadManager::Update() {
    for (auto& c : controllers) {
        if (!c.pad) continue;

        c.state.leftX = normalize(SDL_GetGamepadAxis(c.pad, SDL_GAMEPAD_AXIS_LEFTX));
        c.state.leftY = normalize(SDL_GetGamepadAxis(c.pad, SDL_GAMEPAD_AXIS_LEFTY));
        c.state.rightX = normalize(SDL_GetGamepadAxis(c.pad, SDL_GAMEPAD_AXIS_RIGHTX));
        c.state.rightY = normalize(SDL_GetGamepadAxis(c.pad, SDL_GAMEPAD_AXIS_RIGHTY));

        c.state.leftTrigger = normalize(SDL_GetGamepadAxis(c.pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
        c.state.rightTrigger = normalize(SDL_GetGamepadAxis(c.pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));

        c.state.a = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_SOUTH);
        c.state.b = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_EAST);
        c.state.x = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_WEST);
        c.state.y = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_NORTH);

        c.state.lb = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
        c.state.rb = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);

        c.state.start = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_START);
        c.state.back = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_BACK);

        c.state.ls = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_LEFT_STICK);
        c.state.rs = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_RIGHT_STICK);

        c.state.dpadUp = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_DPAD_UP);
        c.state.dpadDown = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        c.state.dpadLeft = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        c.state.dpadRight = SDL_GetGamepadButton(c.pad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);

        bridge.Update(c.id, c.state);
    }
}