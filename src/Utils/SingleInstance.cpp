#include "SingleInstance.h"

#include <windows.h>

bool SingleInstance::Acquire() {
	mutex_ = CreateMutexA(nullptr, TRUE, "DevlitchInput_SingleInstance");
	if (!mutex_) return false;
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(static_cast <HANDLE> (mutex_));
		mutex_ = nullptr;
		return false;
	}
	return true;
}
void SingleInstance::Release() {
	if (mutex_) {
		//ReleaseMutex(static_cast<HANDLE>(mutex_));
		CloseHandle(static_cast<HANDLE>(mutex_));
		mutex_ = nullptr;
	}
}
SingleInstance::~SingleInstance() {
	Release();
}