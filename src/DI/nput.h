#pragma once

#include "../IPC/TCPClient.h"

#include "../IPC/Shared/Protocol.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>

enum class RequestState {
	Idle,
	Pending,
	Success,
	Failed
};

struct ControllerView {
	IPC::ControllerInfo info{};
	RequestState connectState = RequestState::Idle;
	RequestState pingState = RequestState::Idle;
	RequestState rumbleSwitchState = RequestState::Idle;
	bool vibrationDropdownOpen = false;
};

class DevlitchInput {
public:
	void init();
	bool UpdateControllers();
	void Stop();
private:
	void ResetState();
	enum class ControllerState {
		Connect,
		Disconnect,
		SwitchRumble,
		Ping
	};
	int FindController(uint32_t id);
private:
	//==================================================
	// Main
	//==================================================
	void ReceiveLoop();
	//==================================================
	// IPC Response Handlers
	//==================================================
	void HandleSuccess(const IPC::SuccessPayload& response);
	void HandleError(const IPC::ErrorPayload& response);
	void HandleControllerList(const IPC::ControllerListPayload& response);
	void HandleControllerState(uint32_t controllerId, ControllerState type, bool value = false);
private:
	//==================================================
	// Thread
	//==================================================
	std::thread receiveThread;
	bool di_loading = 0;
public:
	//==================================================
	// UI State
	//==================================================
	bool refreshing = false;
	//==================================================
	// State Mutex
	//==================================================
	std::mutex stateMutex;
	//==================================================
	// Controllers
	//==================================================
	ControllerView controllers[IPC::MAX_CONTROLLERS]{};
};

inline DevlitchInput di;