#include "ViGEmManager.h"
#include <SDL3/SDL.h>
#include <iostream>

ViGEmManager bridge;

bool ViGEmManager::Initialize() {
    client = vigem_alloc();
    if (client == nullptr){
        std::cerr << "Uh, not enough memory to do that?!" << std::endl;
        return false;
    }
    const auto retval = vigem_connect(client);
    if (!VIGEM_SUCCESS(retval)) {
        std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex << retval << std::endl;
        return false;
    }

    std::cout << "ViGEm Ready\n";
    return true;
}

void ViGEmManager::EnsureController(SDL_JoystickID id, SDL_Gamepad* gamepad, bool rumble) {
    if(pads.find(id) != pads.end()) return;

    auto pad = std::make_unique<VirtualPad>();
    pad->gamepad = gamepad;
    pad->rumble = rumble;
    pad->target = vigem_target_x360_alloc();

    VIGEM_ERROR err = vigem_target_add(client, pad->target);

    if(!VIGEM_SUCCESS(err)){
        std::cout << "Failed to create pad for: " << id
            << " error: " << err << std::endl;

        vigem_target_free(pad->target);
        return;
    }

    VirtualPad* virtualPad = pad.get();
    pads.emplace(id, std::move(pad));

    if(rumble) vigem_target_x360_register_notification(client,virtualPad->target,RumbleCallback,virtualPad);

    std::cout << "Created virtual pad for: " << id << std::endl;
}

void ViGEmManager::RemoveController(SDL_JoystickID id) {
    auto it=pads.find(id);
    if(it == pads.end()) return;
    auto& pad = *it->second;
    if(pad.rumble) vigem_target_x360_unregister_notification(pad.target);

    vigem_target_remove(client, pad.target);
    vigem_target_free(pad.target);

    pads.erase(it);

    std::cout << "Virtual pad removed: " << id << std::endl;
}

_Function_class_(EVT_VIGEM_X360_NOTIFICATION)
VOID CALLBACK ViGEmManager::RumbleCallback(
    PVIGEM_CLIENT client,
    PVIGEM_TARGET,
    UCHAR largeMotor,
    UCHAR smallMotor,
    UCHAR,
    LPVOID userData)
{
    auto* pad = static_cast<VirtualPad*>(userData);

    if(!pad) return;
    if(!pad->gamepad) return;
    if(!SDL_GamepadConnected(pad->gamepad)) return;

    pad->rumbleLow = static_cast<Uint16>(smallMotor * 257u);
    pad->rumbleHigh = static_cast<Uint16>(largeMotor * 257u);

    ApplyRumble(pad);
}

void ViGEmManager::ApplyRumble(VirtualPad* pad){
    SDL_RumbleGamepad(pad->gamepad,pad->rumbleLow,pad->rumbleHigh,UINT32_MAX);
    if(pad->rumbleLow==0 && pad->rumbleHigh==0)SDL_RumbleGamepad(pad->gamepad, 0, 0, 50);
}

void ViGEmManager::Update(SDL_JoystickID id, const InputState& s) {
    auto it=pads.find(id);
    if(it == pads.end()) return;
    VirtualPad* pad = it->second.get();

    XUSB_REPORT report{};
    ZeroMemory(&report, sizeof(report));

    report.sThumbLX = (SHORT)(s.leftX * 32767);
    report.sThumbLY = (SHORT)(-s.leftY * 32767);

    report.sThumbRX = (SHORT)(s.rightX * 32767);
    report.sThumbRY = (SHORT)(-s.rightY * 32767);

    report.bLeftTrigger = (BYTE)(s.leftTrigger * 255);
    report.bRightTrigger = (BYTE)(s.rightTrigger * 255);

    if (s.a) report.wButtons |= XUSB_GAMEPAD_A;
    if (s.b) report.wButtons |= XUSB_GAMEPAD_B;
    if (s.x) report.wButtons |= XUSB_GAMEPAD_X;
    if (s.y) report.wButtons |= XUSB_GAMEPAD_Y;

    if (s.lb) report.wButtons |= XUSB_GAMEPAD_LEFT_SHOULDER;
    if (s.rb) report.wButtons |= XUSB_GAMEPAD_RIGHT_SHOULDER;

    if (s.start) report.wButtons |= XUSB_GAMEPAD_START;
    if (s.back) report.wButtons |= XUSB_GAMEPAD_BACK;

    if (s.ls) report.wButtons |= XUSB_GAMEPAD_LEFT_THUMB;
    if (s.rs) report.wButtons |= XUSB_GAMEPAD_RIGHT_THUMB;

    if (s.dpadUp) report.wButtons |= XUSB_GAMEPAD_DPAD_UP;
    if (s.dpadDown) report.wButtons |= XUSB_GAMEPAD_DPAD_DOWN;
    if (s.dpadLeft) report.wButtons |= XUSB_GAMEPAD_DPAD_LEFT;
    if (s.dpadRight) report.wButtons |= XUSB_GAMEPAD_DPAD_RIGHT;

    vigem_target_x360_update(client, pad->target, report);
}

void ViGEmManager::Shutdown() {
    for (auto& [index, pad] : pads) {
        if(pad->rumble) vigem_target_x360_unregister_notification(pad->target);
        vigem_target_remove(client, pad->target);
        vigem_target_free(pad->target);
    }

    pads.clear();

    if(client){
        vigem_disconnect(client);
        vigem_free(client);
        client = nullptr;
    }
}