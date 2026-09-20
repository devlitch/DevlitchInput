#include "Checker.h"

#include <SDL3/SDL.h>
#include <windows.h>

#include <stdexcept>

#include "../Config/Config.h"
#include "../../Utils/Utils.h"
#include "../../GUI/GUI.h"

#include "Reader.h"
#include "Writer.h"
#include "func/SteamInput.h"

using namespace std;
namespace fs = filesystem;

bool Checker::init() {
    gui.LoadingText("Checking DevlitchInput_SP installation");
    std::string shortcutPath = cfg.SteamFolder + "userdata/" + cfg.accountId + "/config/shortcuts.vdf";
    bool exists = true;
    if (!fs::exists(shortcutPath)) {
        exists = false;
        if (!fs::exists(cfg.SteamFolder)) {
            gui.ShowError(
                "Your Steam folder is no longer valid.\n\n"
                "This has caused your config to be reset.\n\n"
                "Please launch the application again."
            );
            cfg.ResetConfig();
            return false;
        }
    }
    vector<Shortcut> shortcuts;
    try
    {
        if(exists) shortcuts = Reader{}.ReadShortcuts(shortcutPath);
    } catch (const exception& e) {
        gui.ShowError("Something went wrong while trying to read your shortcuts.");
        gui.ShowError(string("[shortcuts] ") + e.what());
        gui.Pause();
        return 0;
    }
    string Home = GetProgramFolder();
    string target = Home + "\\sp.exe";
    uint32_t appId = findAppID(target, shortcuts);
    bool check = appId > 0;
    if (!check) {
        gui.LoadingText("Installing DevlitchInput_SP");

        const SDL_MessageBoxButtonData buttons[] = {
            {
                SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
                0,
                "No"
            },
            {
                SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
                1,
                "Yes"
            }
        };

        const SDL_MessageBoxData messageBox = {
            SDL_MESSAGEBOX_INFORMATION,
            gui.window,
            "DevlitchInput - Steam",
            "Steam must be closed to install the program.\n\nDo you want to restart Steam now?",
            SDL_arraysize(buttons),
            buttons,
            nullptr
        };

        int buttonId = 0;

        SDL_ShowMessageBox(&messageBox, &buttonId);

        if (buttonId == 1) {
            gui.BringWindowToFront();
            TerminateSteam();
        } else {
            gui.LoadingText("Exiting");
            gui.ShowError("User cancelled\nInstall it manually or Launch it again");
            gui.Pause();
            return 0;
        }
        Writer w;
        if (!w.CanWriteFile(shortcutPath)) {
            gui.ShowError("No write permission");
            gui.Pause();
            return 0;
        }
        if(exists) w.BackupFile(shortcutPath, shortcutPath +"_backup" + GetCurrentTimeMs());
        Shortcut sc;
        sc.AppName = "DevlitchInput_SP";
        sc.Exe = target;
        sc.StartDir = Home;
        sc.appid = w.GenerateAppID(sc.Exe, sc.AppName);
        appId = sc.appid;
        shortcuts.push_back(sc);
        bool result = findAppID(target, shortcuts) > 0;
        
        if (result) {
            w.WriteShortcuts(shortcutPath, shortcuts);
        } else {
            gui.ShowError("An error occurred while installing DevlitchInput_SP.");
            return false;
        }
    }
    SteamInput steam;
    bool steamInput = steam.CheckSteamControllerConfig(cfg.SteamFolder, cfg.accountId, std::to_string(appId));
    if (!steamInput) {
        gui.LoadingText("Disabling SteamInput");
        if (!steam.SetSteamControllerConfig(cfg.SteamFolder, cfg.accountId, std::to_string(appId))) {
            gui.ShowError("There's an error while turning off the SteamInput");
            gui.Pause();
            return false;
        }
    }
    gui.LoadingText("Starting DevlitchInput_SP");
    if (fs::exists(portPath)) fs::remove(portPath);
    ShellExecuteA(NULL,
        "open",
        ("steam://rungameid/" + std::to_string(AppIDToRunGameID(appId))).c_str(),
        NULL, NULL, SW_SHOWNORMAL
    );
    return true;
}

uint32_t Checker::findAppID(std::string target, std::vector<Shortcut> shortcuts) {
    uint32_t appId = 0;
    for (auto& s : shortcuts) {
        string exe = CleanPath(s.Exe);
        if (exe == target) { appId = s.appid; break; }
    }
    return appId;
}

uint64_t Checker::AppIDToRunGameID(uint32_t appid) {
    return ((uint64_t)appid << 32) | 0x02000000;
}