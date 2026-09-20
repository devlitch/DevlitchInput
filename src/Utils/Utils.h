#pragma once

#include <string>
#include <filesystem>
#include <cstdint>

std::string GetProgramFolder();

std::string CleanPath(std::string path);

std::filesystem::path GetTempDir();
extern std::filesystem::path portPath;

bool IsExeRunning(const std::wstring& exePath);

bool TerminateSteam();

std::string GetCurrentTimeMs();