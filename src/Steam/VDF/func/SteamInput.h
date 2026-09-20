#pragma once

#include <string>
using namespace std;

class SteamInput {
public:
	bool CheckSteamControllerConfig(const string& steamFolder, const string& accountId, const string& appId);
	bool SetSteamControllerConfig(const string& steamFolder, const string& accountId, const string& appId);
private:
	bool func(const string& steamFolder, const string& accountId, const string& appId, bool edit = false);
};