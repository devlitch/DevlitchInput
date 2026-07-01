#include "Tray.h"
#include "../Global/Global.h"
#include <string>
#include <thread>

#define WM_BUILD_MENU (WM_APP + 10)

extern ControllerManager controllerManager;
std::atomic<bool> menuDirty=true;

Tray::Tray(HINSTANCE hInst) {
    this->hInst = hInst;
    hwnd = nullptr;
    ZeroMemory(&nid, sizeof(nid));

    InitWindow();
    InitTray();
}

Tray::~Tray() {
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

void Tray::InitWindow() {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"TrayOnlyClass";

    RegisterClassW(&wc);

    hwnd = CreateWindowW(
        wc.lpszClassName,
        L"",
        0,
        0, 0, 0, 0,
        HWND_MESSAGE,
        nullptr,
        hInst,
        this
    );
}

void Tray::BuildCachedItems(){
    cachedItems.clear();

    const auto& list = controllerManager.GetControllers();

    for (const auto& c : list)
    {
        cachedItems.push_back({
            c.id,
            c.name,
            c.connected
        });
    }
}

HMENU Tray::BuildLoadingMenu() {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 0, L"Building...");
    AppendMenuW(menu, MF_GRAYED, 0, L"Please wait");
    return menu;
}

HMENU Tray::BuildMenuFromCache(){
    HMENU menu = CreatePopupMenu();

    if (cachedItems.empty()){
        AppendMenuW(menu, MF_STRING, 0, L"---None---");
    }else{
        for (const auto& item : cachedItems){
            UINT flags = MF_STRING | (item.checked ? MF_CHECKED : MF_UNCHECKED);

            AppendMenuW(
                menu,
                flags,
                5000 + item.id,
                item.text.c_str()
            );
        }
    }

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 1, L"Refresh");
    AppendMenuW(menu, MF_STRING, 2, L"Close");

    return menu;
}

void Tray::InitTray() {
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = (HICON)LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(101), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
    wcscpy_s(nid.szTip, L"DevlitchInput");

    Shell_NotifyIconW(NIM_ADD, &nid);
    BuildCachedItems();
    cachedMenu = BuildMenuFromCache();
}

void Tray::Run() {
    MSG msg;

    while (GetMessageW(&msg, nullptr, 0, 0)) {
        if (menuDirty&&!Building) {
            Building = true;
            BuildCachedItems();
            if (cachedMenu) {
                DestroyMenu(cachedMenu);
                cachedMenu = nullptr;
            }
            cachedMenu = BuildMenuFromCache();
            Building = false;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}


LRESULT CALLBACK Tray::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg){
    case WM_CREATE:
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        Tray* self = (Tray*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
        return 0;
    }
    case WM_APP + 1:
    {
        Tray* self = (Tray*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

        if (self->Building) return 0;

        self->Building=true;
        menuDirty = true;

        std::thread([self, hwnd]() {
            self->BuildCachedItems();
            PostMessage(hwnd, WM_BUILD_MENU, 0, 0);
        }).detach();
        return 0;
    }
    case WM_BUILD_MENU:
    {
        Tray* self = (Tray*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

        if (self->cachedMenu){
            DestroyMenu(self->cachedMenu);
            self->cachedMenu = nullptr;
        }

        self->cachedMenu = self->BuildMenuFromCache();

        menuDirty = false;
        self->Building = false;

        return 0;
    }
    case WM_APP + 2:
    {
        SDL_JoystickID controllerId = static_cast<SDL_JoystickID>(wParam);
        Tray* self = (Tray*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (self->Building) return 0;
        self->Building = true;
        std::thread([self, controllerId, hwnd]() {
            controllerManager.toggleControllerState(controllerId);
            self->Building = false;
            PostMessage(hwnd, WM_APP + 1, 0, 0);
            }).detach();
        return 0;
    }
    case WM_APP + 3:
    {
        SDL_JoystickID controllerId = static_cast<SDL_JoystickID>(wParam);
        Tray* self = (Tray*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (self->Building) return 0;
        self->Building = true;
        std::thread([self, controllerId, hwnd]() {
            controllerManager.Refresh();
            self->Building = false;
            PostMessage(hwnd, WM_APP + 1, 0, 0);
        }).detach();
        return 0;
    }
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            HMENU menu;
            Tray* self = (Tray*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (!self) return DefWindowProcW(hwnd, msg, wParam, lParam);

            if (menuDirty && !self->Building) PostMessage(hwnd, WM_APP + 1, 0, 0);

            menu = self->Building ? self->BuildLoadingMenu() : self->cachedMenu;
            if (!menu) return 0;

            POINT pt;
            GetCursorPos(&pt);

            SetForegroundWindow(hwnd);

            TrackPopupMenu(
                menu,
                TPM_RIGHTBUTTON | TPM_TOPALIGN,
                pt.x,
                pt.y,
                0,
                hwnd,
                nullptr
            );
            if(self->Building) DestroyMenu(menu);
            PostMessage(hwnd, WM_NULL, 0, 0);
        }
        return 0;

    case WM_COMMAND:
        int id = LOWORD(wParam);

        if (id >= 5000) {
            int controllerId = id - 5000;

            PostMessage(hwnd, WM_APP + 2, controllerId, 0);

            return 0;
        }

        switch (id) {
            case 1:
                PostMessage(hwnd, WM_APP + 3, 0, 0);
                break;

            case 2:
                PostQuitMessage(0);
                g_Running = false;
                break;
        }
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}