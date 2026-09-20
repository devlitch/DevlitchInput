#pragma once

#include "SteamUser.h"
#include "../../GUI/Renderer.h"
#include <string>
#include <vector>

class UserSelector {
public:
	void init();

private:
	std::vector<SteamUser> users;
	Texture* LoadAvatar(Renderer& renderer, std::string& file);
	void LoadAvatars(Renderer& renderer);
	void Render();
};