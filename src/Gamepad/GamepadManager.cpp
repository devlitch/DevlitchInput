#include "GamepadManager.h"
#include "../Bridge/ViGEmManager.h"
#include "../Global/Global.h"
#include <iostream>
#include <algorithm>

GamepadManager gamepadManager;

extern ControllerManager controllerManager;
extern ViGEmManager bridge;

bool GamepadManager::Initialize() {
    if (SDL_Init(SDL_INIT_GAMEPAD) < 0) {
        std::cout << "SDL init failed\n";
        return false;
    }

    std::cout << "SDL Initialized\n";
    return true;
}

void GamepadManager::addController(SDL_JoystickID id) {
    Controller* cId = find(id);
    if (cId) return;

    SDL_Gamepad* pad = SDL_OpenGamepad(id);
    if (!pad) {
        printf("OpenGamepad(%u) failed: %s\n", id, SDL_GetError());
        return;
    }

    Controller c;
    c.pad = pad;
    c.id = id;
    controllers.push_back(c);
    bridge.EnsureController(id);
}

void GamepadManager::removeController(SDL_JoystickID id) {
    Controller* cId = find(id);
    if (!cId) return;

    bridge.RemoveController(id);

    controllers.erase(
        std::remove_if(controllers.begin(), controllers.end(), [&](Controller& c) {
                return c.id == id;
        }), controllers.end()
    );

    std::cout << "Controller removed: " << id << std::endl;
}

Controller* GamepadManager::find(SDL_JoystickID id) {
    for (auto& c : controllers) {
        if (c.id == id)
            return &c;
    }
    return nullptr;
}

float GamepadManager::normalize(int value) {
    return value / 32767.0f;
}

void GamepadManager::Update() {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {

        // Exit         #CTRL + C
        if (e.type == 256) {
            std::cout << "Exiting...\n";
            g_Running = false;
            return;
        }

        // CONNECT
        if (e.type == SDL_EVENT_JOYSTICK_ADDED) {
            controllerManager.Refresh();
        }

        // DISCONNECT
        if (e.type == SDL_EVENT_JOYSTICK_REMOVED) {
            SDL_JoystickID id = e.gdevice.which;

            removeController(id);
            
            controllerManager.Refresh();
        }
    }

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