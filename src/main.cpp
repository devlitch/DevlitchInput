#include <string>
#include <cstdlib>

#include "GUI/GUI.h"
#include "GUI/Tabs.h"
#include "Steam/Config/Config.h"
#include "Steam/Select/Folder.h"
#include "Steam/Login/Selector.h"
#include "Steam/VDF/checker.h"
#include "DI/nput.h"

int main() {
    gui.Init();
    
    cfg.LoadConfig();

    if (cfg.SteamFolder.empty()) {
        SelectSteamFolder{}.init();
    }

    if (cfg.User.empty() && !cfg.SteamFolder.empty()) {
        UserSelector{}.init();
    }

    if(!cfg.User.empty() && !cfg.SteamFolder.empty()){
        if (Checker{}.init()) {
            if (client.init()) {
                gui.RaiseWindow();
                di.init();
                tab.Render();
                di.Stop();
            }
        }
    }

    if (gui.isActive()) gui.Stop();
    if (gui.guiThread.joinable()) gui.guiThread.join();
    return 290;
}