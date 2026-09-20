#include "SteamInput.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>

#include "vdf_parser.hpp"

#include "../../../Utils/Utils.h"
#include "../../../GUI/GUI.h"

namespace fs = filesystem;

bool SteamInput::SetSteamControllerConfig(const string& steamFolder, const string& accountId, const string& appId) {
	return func(steamFolder, accountId, appId, true);
}

bool SteamInput::CheckSteamControllerConfig(const string& steamFolder, const string& accountId, const string& appId) {
	return func(steamFolder, accountId, appId);
}

bool SteamInput::func(const string& steamFolder, const string& accountId, const string& appId, bool edit) {
	using namespace tyti::vdf;
	try {
		const auto filePath = fs::path(steamFolder) / "userdata" / accountId / "config" / "localconfig.vdf";
		if (!fs::exists(filePath)) {
			ofstream file(filePath);
			if (!file.is_open()) {
				gui.ShowError("[steam_part] Failed to create: " + filePath.string());
				return false;
			}
			file << "\"UserLocalConfigStore\"\n"
				"{\n"
				"    \"apps\"\n"
				"    {\n"
				"    }\n"
				"}\n";
		}
		object root;
		{
			ifstream file(filePath);
			if (!file.is_open()) {
				gui.ShowError("[steam_part] Failed to open: " + filePath.string());
				return false;
			}
			root = read(file);
		}

		if (root.name != "UserLocalConfigStore") {
			gui.ShowError("[steam_part/localconfig.vdf] Unexpected root object: " + root.name);
			return false;
		}

		shared_ptr<object> apps;
		auto appsIt = root.childs.find("apps");
		if (appsIt == root.childs.end()) {
			apps = make_shared<object>();
			apps->name = "apps";
			root.childs.emplace("apps", apps);
		} else {
			apps = appsIt->second;
		}

		shared_ptr<object> app;
		auto appIt = apps->childs.find(appId);
		if (appIt == apps->childs.end()) {
			app = std::make_shared<object>();
			app->name = appId;
			apps->childs.emplace(appId, app);
		} else {
			app = appIt->second;
		}

		if (!edit) {
			auto it = app->attribs.find("UseSteamControllerConfig");

			if (it != app->attribs.end()) {
				if (app->attribs["UseSteamControllerConfig"] == "0") {
					return app->attribs["UseSteamControllerConfig"] == "0" ? true : false;
				}
			} else {
				return false;
			}
		}

		app->attribs["UseSteamControllerConfig"] = "0";
		app->attribs["SteamControllerRumble"] = "1";
		app->attribs["SteamControllerRumbleIntensity"] = "320";

		const auto tempPath = filePath.string() + ".tmp";
		{
			ofstream out(tempPath, ios::trunc);
			if (!out.is_open()) {
				gui.ShowError("[steam_part] Failed to create temporary file. [localconfig.vdf]");
				return false;
			}
			write(out, root);
		}

		error_code ec;
		fs::rename(filePath, filePath.string() + "_backup" + GetCurrentTimeMs(), ec);
		if (ec) {
			gui.ShowError("[steam_part/localconfig.vdf] Can't rename original file: " + ec.message());
			fs::remove(tempPath);
			return false;
		}
		fs::rename(tempPath, filePath, ec);
		if (ec) {
			gui.ShowError("[steam_part/localconfig.vdf] Failed to replace original file: " + ec.message());
			return false;
		}
		return true;
	} catch (const exception& e) {
		gui.ShowError(string("[steam_part/localconfig.vdf] Exception: ") + e.what());
		return false;
	}
}