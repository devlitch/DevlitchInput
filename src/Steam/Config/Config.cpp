#include "Config.h"

#include "../../GUI/Loading.h"

#include <fstream>
#include <json.hpp>
#include <filesystem>

using json = nlohmann::json;
std::string configFile = "Config.json";

bool Config::LoadConfig() {
    Loading::Text("Loading config");
    std::ifstream in(configFile);

    if (!in.is_open()) { SaveConfig(); return LoadConfig(); }
    json j;
    in >> j;

    User = j.value("User", "");
    accountId = j.value("accountId", "");
    SteamFolder = j.value("SteamFolder", "");

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

    std::ofstream out(configFile);

    if (!out.is_open()) return false;

    out << j.dump(4);

    return true;
}

bool Config::ResetConfig() {
    User = "";
    accountId = "";
    SteamFolder = "";

    return SaveConfig();
}