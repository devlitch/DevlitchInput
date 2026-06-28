#include "ViGEmManager.h"
#include <iostream>

ViGEmManager bridge;

bool ViGEmManager::Initialize() {
    client = vigem_alloc();
    if (!client) return false;

    if (!VIGEM_SUCCESS(vigem_connect(client))) return false;

    std::cout << "ViGEm Ready\n";
    return true;
}

void ViGEmManager::EnsureController(SDL_JoystickID id) {
    if (pads.find(id) != pads.end()) return;

    const char* path = SDL_GetJoystickPathForID(id);
    if (!path)  return;

    std::cout << "Device Path: " << path << std::endl;

    PVIGEM_TARGET pad = vigem_target_x360_alloc();

    VIGEM_ERROR err = vigem_target_add(client, pad);

    if (!VIGEM_SUCCESS(err)) {
        std::cout << "Failed to create pad for: " << id
            << " error: " << err << std::endl;

        vigem_target_free(pad);
        return;
    }

    pads[id] = pad;

    std::cout << "Created virtual pad for: " << id << std::endl;
}

void ViGEmManager::RemoveController(SDL_JoystickID id) {
    auto it=pads.find(id);
    if (it == pads.end()) return;

    vigem_target_remove(client, it->second);
    vigem_target_free(it->second);

    pads.erase(it);

    std::cout << "Virtual pad removed: " << id << std::endl;
}

void ViGEmManager::Update(SDL_JoystickID id, const InputState& s) {
    auto it=pads.find(id);
    if(it == pads.end()) return;

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

    vigem_target_x360_update(client, it->second, report);
}

void ViGEmManager::Shutdown() {
    for (auto& [index, pad] : pads) {
        vigem_target_remove(client, pad);
        vigem_target_free(pad);
    }

    pads.clear();

    if (client) {
        vigem_disconnect(client);
        vigem_free(client);
        client = nullptr;
    }
}