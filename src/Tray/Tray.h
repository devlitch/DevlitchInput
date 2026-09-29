#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <commctrl.h>

#include <functional>

#define IDI_APP_ICON 101

class TrayManager
{
public:
    static constexpr UINT WM_TRAYICON = WM_APP + 1;

    TrayManager() = default;
    ~TrayManager();

    bool Create(HWND hwnd);
    void Destroy();

    void SetOnExit(std::function<void()> callback);
    void SetOnShow(std::function<void()> callback);

private:
    static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

    void HandleMessage(WPARAM wParam, LPARAM lParam);

    void ShowContextMenu();
    void ExitApplication();
    void ShowApplication();

private:
    HWND m_hwnd = nullptr;

    NOTIFYICONDATAW m_nid{};
    HMENU m_menu = nullptr;
    HICON m_icon = nullptr;

    std::function<void()> m_onExit;
    std::function<void()> m_onShow;

    static constexpr UINT TRAY_ICON_ID = 1;
    static constexpr UINT ID_TRAY_EXIT = 1001;
    static constexpr UINT_PTR SUBCLASS_ID = 0xD1;
};

inline TrayManager tray;