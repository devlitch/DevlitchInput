#include <iostream>
#include <thread>

#include "Gamepad/GamepadManager.h"
#include "Bridge/ViGEmManager.h"
#include "Tray/Tray.h"
#include "Global/Global.h"

extern ViGEmManager bridge;
extern GamepadManager gamepadManager;

int main() {
    if (!bridge.Initialize()) {
        std::cout << "ViGEm init failed\n";
        return -1;
    }

    if (!gamepadManager.Initialize()) {
        std::cout << "SDL failed\n";
        return -1;
    }


    std::thread([] {
        Tray tray(GetModuleHandleW(nullptr));
        tray.Run();
    }).detach();

    std::cout << "Running...\n";

    while (g_Running) {
        gamepadManager.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    bridge.Shutdown();

    return 0;
}