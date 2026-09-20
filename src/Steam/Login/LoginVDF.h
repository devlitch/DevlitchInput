#pragma once

#include "SteamUser.h"
#include <string>
#include <vector>

class LoginVDF {
public:
	std::vector<SteamUser> LoadSteamUsers(const std::string& steamFolder);
};