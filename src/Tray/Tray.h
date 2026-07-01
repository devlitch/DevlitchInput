#pragma once

#include <Windows.h>
#include <shellapi.h>
#include "../Input/ControllerManager.h"
#include <vector>
#include <string>

#pragma comment(lib, "Shell32.lib")

#define WM_TRAYICON (WM_USER + 1)

struct MenuItem {
    SDL_JoystickID id;
    std::wstring text;
    bool checked;
};

extern std::atomic<bool> menuDirty;

class Tray
{
public:
    Tray(HINSTANCE hInst);
    ~Tray();

    void Run();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void InitWindow();
    void InitTray();

    void BuildCachedItems();
    HMENU BuildLoadingMenu();
    HMENU BuildMenuFromCache();

    HMENU cachedMenu = nullptr;
    bool Building = false;

    std::vector<MenuItem> cachedItems;

    HINSTANCE hInst;
    HWND hwnd;
    NOTIFYICONDATAW nid;
};