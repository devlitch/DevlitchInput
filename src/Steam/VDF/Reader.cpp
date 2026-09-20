#include "Reader.h"

#include <cstdint>
#include <stdexcept>

using namespace std;

void Reader::ReadShortcut(ReaderFunc& r, Shortcut& sc) {
    while (true) {
        uint8_t type = r.Type();

        if (type == VDF_END)
            return;

        string key = r.String();

        switch (type) {
            case VDF_STRING: {
                string value = r.String();
                if (key == "AppName") sc.AppName = value;
                else if (key == "Exe") sc.Exe = value;
                else if (key == "StartDir") sc.StartDir = value;
                else if (key == "icon") sc.Icon = value;
                else if (key == "ShortcutPath") sc.ShortcutPath = value;
                else if (key == "LaunchOptions") sc.LaunchOptions = value;
                else if (key == "DevkitGameID") sc.DevkitGameID = value;
                else if (key == "FlatpakAppID") sc.FlatpakAppID = value;
                else if (key == "sortas") sc.SortAs = value;
                break;
            }
            case VDF_INT: {
                uint32_t value = r.Int();
                if (key == "appid") sc.appid = value;

                else if (key == "IsHidden") sc.IsHidden = value != 0;
                else if (key == "AllowDesktopConfig") sc.AllowDesktopConfig = value != 0;
                else if (key == "AllowOverlay") sc.AllowOverlay = value != 0;
                else if (key == "OpenVR") sc.OpenVR = value != 0;
                else if (key == "Devkit") sc.Devkit = value != 0;
                else if (key == "DevkitOverrideAppID") sc.DevkitOverrideAppID = value;
                else if (key == "LastPlayTime") sc.LastPlayTime = value;
                break;
            }
            case VDF_DICT: {
                if (key == "tags") {
                    while (true) {
                        uint8_t t = r.Type();
                        if (t == VDF_END) break;
                        if (t != VDF_STRING) break;
                        r.String();
                        string tag = r.String();
                        sc.Tags.push_back(tag);
                    }
                } else {
                    int depth = 1;
                    while (depth > 0) {
                        uint8_t t = r.Type();
                        if (t == VDF_END) {
                            depth--;
                        } else {
                            r.String();
                            if (t == VDF_DICT) depth++;
                            else if (t == VDF_STRING) r.String();
                            else if (t == VDF_INT) r.Int();
                        }
                    }
                }
                break;
            }
        }
    }
}

vector<Shortcut> Reader::ReadShortcuts(string path) {
    vector<Shortcut> result;
    ReaderFunc r;
    if (!r.Open(path)) {
        throw std::runtime_error("Failed to open shortcuts.vdf");
        return result;
    }
    uint8_t type = r.Type();
    if (type != VDF_DICT) {
        throw std::runtime_error("Invalid VDF file");
        return result;
    }
    string root = r.String();
    if (root != "shortcuts") {
        throw std::runtime_error("Not shortcuts.vdf");
        return result;
    }
    while (true) {
        uint8_t t = r.Type();
        if (t == VDF_END) break;
        if (t != VDF_DICT) break;
        r.String();
        Shortcut sc;
        ReadShortcut(r, sc);
        result.push_back(sc);
    }
    return result;
}