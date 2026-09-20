#pragma once

#include "../IPC/TCPClient.h"

#include "../IPC/Shared/Protocol.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>

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
		Ping
	};
	int FindController(IPC::ControllerInfo* controllers, uint32_t id);
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
	void HandleControllerState(uint32_t controllerId, ControllerState type);
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
	IPC::ControllerInfo controllers[IPC::MAX_CONTROLLERS]{};
	//==================================================
	// Request State
	//==================================================
	IPC::RequestState connectState[IPC::MAX_CONTROLLERS]{};
	IPC::RequestState pingState[IPC::MAX_CONTROLLERS]{};
};

inline DevlitchInput di;