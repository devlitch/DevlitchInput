#include "Tray.h"
#include "../Global/Global.h"

extern ControllerManager controllerManager;

Tray::Tray(HINSTANCE hInst) {
    this->hInst = hInst;
    hwnd = nullptr;
    ZeroMemory(&nid, sizeof(nid));

    InitWindow();
    InitMenu();
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
        nullptr
    );
}

void Tray::InitMenu()
{
    hMenu = CreatePopupMenu();

    AppendMenuW(hMenu, MF_STRING, 1, L"Settings");
    AppendMenuW(hMenu, MF_STRING, 2, L"Close");
}

void Tray::InitTray() {
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;

    nid.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"My Tray App");

    Shell_NotifyIconW(NIM_ADD, &nid);
}

void Tray::Run() {
    MSG msg;

    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

LRESULT CALLBACK Tray::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HMENU hMenu = nullptr;
    static NOTIFYICONDATAW* pNid = nullptr;

    switch (msg)
    {
    case WM_CREATE:
        break;

    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            HMENU menu = CreatePopupMenu();

            const auto& list = controllerManager.GetControllers();
            if ((int)list.size() == 0) {
                AppendMenuW(menu, MF_STRING, 0, L"---None---");
            }
            else {
                for (int i = 0; i < (int)list.size(); i++) {
                    const auto& c = list[i];

                    UINT flags = MF_STRING | (c.connected ? MF_CHECKED : MF_UNCHECKED);

                    AppendMenuA(
                        menu,
                        flags,
                        5000 + c.id,
                        c.name.c_str()
                    );
                }
            }

            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(menu, MF_STRING, 1, L"Refresh");
            AppendMenuW(menu, MF_STRING, 2, L"Close");

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
            PostMessage(hwnd, WM_NULL, 0, 0);
        }
        return 0;

    case WM_COMMAND:
        int id = LOWORD(wParam);

        if (id >= 5000) {
            int controllerId = id - 5000;

            controllerManager.toggleControllerState(controllerId);

            return 0;
        }

        switch (id) {
        case 1:
            controllerManager.Refresh();
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