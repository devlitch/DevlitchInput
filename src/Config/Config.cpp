#include "Config.h"

#include "../GUI/Loading.h"
#include "../GUI/GUI.h"

#include <fstream>
#include <json.hpp>
#include <filesystem>

using json = nlohmann::json;
std::string configFile = "Config.json";

bool Config::LoadConfig() {
    Loading::Text("Loading config");
    std::ifstream in(configFile);

    if (!in.is_open()) {
        if (!SaveConfig()) {
            gui.ShowError("Failed to create the configuration file.\n\nThe application cannot continue.");
            return false;
        }
        return LoadConfig();
    }
    json j;
    in >> j;

    User = j.value("User", "");
    accountId = j.value("accountId", "");
    SteamFolder = j.value("SteamFolder", "");

    if (j.contains("GUI") && j["GUI"].is_object()) {
        gui.options.minimizeToTray = j["GUI"].value("minimizeToTray", false);
        gui.options.closeToTray = j["GUI"].value("closeToTray", false);
    }

    if (SteamFolder.empty()) {
        if (std::filesystem::exists("C:\\Program Files (x86)\\Steam\\")) SteamFolder = "C:\\Program Files (x86)\\Steam\\";
    }

    return true;
}

bool Config::SaveConfig() {
    json j;

    j["User"] = User;
    j["accountId"] = accountId;
    j["SteamFolder"] = SteamFolder;

    j["GUI"]["minimizeToTray"] = gui.options.minimizeToTray;
    j["GUI"]["closeToTray"] = gui.options.closeToTray;

    std::ofstream out(configFile);

    if (!out.is_open()) return false;

    out << j.dump(4);

    return true;
}

bool Config::ResetSteamConfig(bool t) {
    User = "";
    accountId = "";
    if (t) SteamFolder = "";

    return SaveConfig();
}