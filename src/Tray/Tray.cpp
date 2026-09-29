#include "Tray.h"

TrayManager::~TrayManager() {
	Destroy();
}

bool TrayManager::Create(HWND hwnd) {
	if (!hwnd) return false;
	if (m_hwnd) return true;
	m_hwnd = hwnd;
	// =========================================================
	// Load application icon from .rc
	// =========================================================
	m_icon = static_cast <HICON> (LoadImageW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDI_APP_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
	if (!m_icon) {
		m_icon = LoadIconW(nullptr, IDI_APPLICATION);
	}
	// =========================================================
	// Create tray icon
	// =========================================================
	m_nid = {};
	m_nid.cbSize = sizeof(NOTIFYICONDATAW);
	m_nid.hWnd = m_hwnd;
	m_nid.uID = TRAY_ICON_ID;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_TRAYICON;
	m_nid.hIcon = m_icon;
	wcscpy_s(m_nid.szTip, L"DevlitchInput v1.6");
	if (!Shell_NotifyIconW(NIM_ADD, &m_nid)) {
		if (m_icon) {
			DestroyIcon(m_icon);
			m_icon = nullptr;
		}
		m_hwnd = nullptr;
		return false;
	}
	// =========================================================
	// Create context menu
	// =========================================================
	m_menu = CreatePopupMenu();
	if (!m_menu) {
		Shell_NotifyIconW(NIM_DELETE, &m_nid);
		if (m_icon) {
			DestroyIcon(m_icon);
			m_icon = nullptr;
		}
		m_nid = {};
		m_hwnd = nullptr;
		return false;
	}
	AppendMenuW(m_menu, MF_STRING | MF_DISABLED, 0, L"DevlitchInput v1.6");
	AppendMenuW(m_menu, MF_SEPARATOR, 0, nullptr);
	AppendMenuW(m_menu, MF_STRING, ID_TRAY_EXIT, L"Exit");
	// =========================================================
	// Subclass SDL's HWND
	// =========================================================
	if (!SetWindowSubclass(m_hwnd, SubclassProc, SUBCLASS_ID, reinterpret_cast <DWORD_PTR> (this))) {
		Destroy();
		return false;
	}
	return true;
}
void TrayManager::Destroy() {
	if (m_hwnd) {
		RemoveWindowSubclass(m_hwnd, SubclassProc, SUBCLASS_ID);
	}
	if (m_menu) {
		DestroyMenu(m_menu);
		m_menu = nullptr;
	}
	if (m_nid.hWnd) {
		Shell_NotifyIconW(NIM_DELETE, &m_nid);
		m_nid = {};
	}
	if (m_icon) {
		DestroyIcon(m_icon);
		m_icon = nullptr;
	}
	m_hwnd = nullptr;
}
void TrayManager::SetOnExit(std::function<void()> callback) {
	m_onExit = std::move(callback);
}
void TrayManager::SetOnShow(std::function<void()> callback) {
	m_onShow = std::move(callback);
}
// =============================================================
// Win32 subclass procedure
// =============================================================
LRESULT CALLBACK TrayManager::SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	auto* tray = reinterpret_cast <TrayManager*> (dwRefData);
	if (tray) {
		if (msg == WM_TRAYICON) {
			tray->HandleMessage(wParam, lParam);
			return 0;
		}
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
}
// =============================================================
// Tray message handling
// =============================================================
void TrayManager::HandleMessage(WPARAM wParam, LPARAM lParam) {
	if (wParam != TRAY_ICON_ID) return;
	switch (static_cast <UINT> (lParam)) {
	case WM_RBUTTONUP: {
		ShowContextMenu();
		break;
	}
	case WM_LBUTTONDBLCLK: {
		ShowApplication();
		break;
	}
	default:
		break;
	}
}
// =============================================================
// Context menu
// =============================================================
void TrayManager::ShowContextMenu() {
	if (!m_menu) return;
	POINT point{};
	if (!GetCursorPos(&point)) return;
	const int command = TrackPopupMenu(m_menu, TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, 0, m_hwnd, nullptr);
	switch (command) {
	case ID_TRAY_EXIT: {
		ExitApplication();
		break;
	}
	default:
		break;
	}
	if (m_hwnd) PostMessageW(m_hwnd, WM_NULL, 0, 0);
}
// =============================================================
// Exit
// =============================================================
void TrayManager::ExitApplication() {
	if (m_onExit) m_onExit();
}
// =============================================================
// Show application
// =============================================================
void TrayManager::ShowApplication() {
	if (m_onShow) m_onShow();
}