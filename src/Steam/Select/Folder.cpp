#include "Folder.h"

#include <array>

#include "../../GUI/GUI.h"
#include "../Config/Config.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_dialog.h>

void SelectSteamFolder::init() {
	if (getSteamFolder()) {
		fs::path steamFolder = GetPath()->string();
		if (!isSteamFolder(steamFolder)) {
			gui.ShowError("Your current directory is not steam");
		} else {
			cfg.SteamFolder = steamFolder.string() + static_cast<char>(fs::path::preferred_separator);
			SDL_Log("Selected: %s", cfg.SteamFolder.c_str());
			cfg.SaveConfig();
		}
	}
}

bool SelectSteamFolder::isSteamFolder(const fs::path& folder) {
	const array<fs::path, 4> requiredPaths = {
		"steam.exe",
		"config",
		"config/loginusers.vdf",
		"userdata"
	};

	for (const auto& required : requiredPaths) {
		if (!fs::exists(folder / required)) return false;
	}

	return true;
}

bool SelectSteamFolder::getSteamFolder() {
	{
		lock_guard<mutex> lock(Mutex);
		selectedPath.reset();
		completed = false;
	}
	SDL_ShowOpenFolderDialog(FolderCallback, this, nullptr, nullptr, false);
	unique_lock<mutex> lock(Mutex);
	cv.wait(lock, [this] {
		return completed;
	});
	return selectedPath.has_value();
}

void SDLCALL SelectSteamFolder::FolderCallback(void* userdata, const char*
	const* filelist, int filter) {
	auto* self = static_cast<SelectSteamFolder*> (userdata);
	if (!self) return;
	{
		lock_guard<mutex> lock(self->Mutex);
		if (filelist && *filelist) {
			self->selectedPath = fs::path(*filelist);
		}
		self->completed = true;
	}
	self->cv.notify_one();
}

const optional<fs::path>& SelectSteamFolder::GetPath() const {
	return selectedPath;
}