#include <string>
#include <cstdlib>

#include "GUI/GUI.h"
#include "Config/Config.h"
#include "Steam/Select/Folder.h"
#include "Steam/Login/Selector.h"
#include "Steam/VDF/checker.h"
#include "DI/nput.h"
#include "GUI/Tabs.h"
#include "Utils/SingleInstance.h"

int main() {
    bool run = true;

    SingleInstance singleInstance;
    if (!singleInstance.Acquire()) {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "DevlitchInput Error",
            "DevlitchInput is already running.",
            nullptr
        );
        run = false;
    }

    if(run) gui.Init();

    if (run && !cfg.LoadConfig()) {
        run = false;
    }

    if (run && cfg.SteamFolder.empty()) {
        SelectSteamFolder{}.init();
        if (cfg.SteamFolder.empty()) run = false;
    }

    if (run && cfg.User.empty()) {
        UserSelector{}.init();
        if (cfg.User.empty()) run = false;
    }

    if(run && Checker{}.init() && client.init()){
        gui.RaiseWindow();
        di.init();
        tab.init();
        di.Stop();
    }

    if (gui.isActive()) gui.Stop();
    if (gui.guiThread.joinable()) gui.guiThread.join();
    return 290;
}