#include "Utils.h"

#include <chrono>
#include <windows.h>
#include <tlhelp32.h>
#include <algorithm>

std::string GetProgramFolder() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL,buffer,MAX_PATH);
    std::string path(buffer);
    size_t pos = path.find_last_of("\\/");
    return path.substr(0, pos);
}

std::string CleanPath(std::string path){
    if (!path.empty() && path.front() == '"') path.erase(0, 1);
    if (!path.empty() && path.back() == '"') path.pop_back();
    return path;
}

std::filesystem::path GetTempDir() {
    return std::filesystem::temp_directory_path();
}

std::filesystem::path portPath = GetTempDir() / "DevlitchInput_Port";

bool IsExeRunning(const std::wstring& exePath) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
            if (hProcess) {
                wchar_t path[MAX_PATH];
                DWORD size = MAX_PATH;

                if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
                    if (_wcsicmp(path, exePath.c_str()) == 0) {
                        CloseHandle(hProcess);
                        CloseHandle(snap);
                        return true;
                    }
                }
                CloseHandle(hProcess);
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return false;
}

bool TerminateSteam() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);

    bool terminated = false;

    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, L"steam.exe") == 0) {
                HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);

                if (process) {
                    terminated = TerminateProcess(process, 0) != FALSE;
                    CloseHandle(process);
                }
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return terminated;
}

std::string GetCurrentTimeMs() {
    return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count());
}