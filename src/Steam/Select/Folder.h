#pragma once

#include <filesystem>
#include <optional>
#include <condition_variable>
#include <mutex>

#include <SDL3/SDL.h>

using namespace std;
namespace fs = filesystem;

class SelectSteamFolder {
public:
    void init();

private:
    bool isSteamFolder(const fs::path& folder);
    bool getSteamFolder();
    const optional<fs::path>& GetPath() const;
    static void SDLCALL FolderCallback(void* userdata, const char* const* filelist, int filter);

private:
    optional<fs::path> selectedPath;

    mutex Mutex;
    condition_variable cv;
    bool completed = false;
};