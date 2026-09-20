#include "LoginVDF.h"

#include <filesystem>
#include <fstream>
#include <string>

#include "vdf_parser.hpp"

std::vector<SteamUser> LoginVDF::LoadSteamUsers(const std::string& steamFolder) {
	std::vector<SteamUser> users;
	const auto path = std::filesystem::path(steamFolder) / "config" / "loginusers.vdf";
	std::ifstream file(path);
	if (!file) return users;
	try {
		auto root = tyti::vdf::read(file);
		if (root.name != "users") return users;
		for (const auto& [steamId, userObject] : root.childs) {
			if (!userObject) continue;
			SteamUser user{};
			// SteamID64
			try {
				user.id = std::stoull(steamId);
			} catch (...) {
				continue;
			}
			// PersonaName
			auto personaIt = userObject->attribs.find("PersonaName");
			if (personaIt != userObject->attribs.end()) {
				user.name = personaIt->second;
			}
			// Avatar
			const auto avatarPath = std::filesystem::path(steamFolder) / "config" / "avatarcache" / (std::to_string(user.id) + ".png");
			if (std::filesystem::exists(avatarPath)) {
				user.avatarUrl = avatarPath.string();
				user.local = 1;
			} else {
				user.avatarUrl.clear();
				user.local = 0;
			}
			users.push_back(std::move(user));
		}
	} catch (...) {
		return {};
	}
	return users;
}