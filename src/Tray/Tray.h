#pragma once

#include <Windows.h>
#include <shellapi.h>
#include "../Input/ControllerManager.h"

#pragma comment(lib, "Shell32.lib")

#define WM_TRAYICON (WM_USER + 1)

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
    void InitMenu();

private:
    HINSTANCE hInst;
    HWND hwnd;
    NOTIFYICONDATAW nid;
    HMENU hMenu;
};