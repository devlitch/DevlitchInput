#pragma once

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <ViGEm/Client.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <SDL3/SDL_gamepad.h>

#include "../InputState.h"

struct VirtualPad
{
    PVIGEM_TARGET target = nullptr;
    SDL_Gamepad* gamepad = nullptr;
    bool rumble = 0;
    Uint16 rumbleLow = 0;
    Uint16 rumbleHigh = 0;
};

class ViGEmManager
{
public:
    bool Initialize();
    void Shutdown();

    bool EnsureController(SDL_JoystickID id, SDL_Gamepad* gamepad, bool rumble);
    void RemoveController(SDL_JoystickID id);
    bool isOurDevice(USHORT vid, USHORT pid) const;

    void Update(SDL_JoystickID id, const InputState& state);

private:
    PVIGEM_CLIENT client = nullptr;

    _Function_class_(EVT_VIGEM_X360_NOTIFICATION)
    static VOID CALLBACK RumbleCallback(
        PVIGEM_CLIENT client,
        PVIGEM_TARGET target,
        UCHAR largeMotor,
        UCHAR smallMotor,
        UCHAR ledNumber,
        LPVOID userData
    );
    static void ApplyRumble(VirtualPad* pad);

    std::unordered_map<SDL_JoystickID, std::unique_ptr<VirtualPad>> pads;

    static constexpr USHORT MY_VID = 0xDF69;
    static constexpr USHORT MY_PID = 0x69FA;
};

inline ViGEmManager bridge;