#include "nput.h"

#include <SDL3/SDL.h>

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../GUI/GUI.h"
#include "../GUI/Notification.h"

void DevlitchInput::ResetState() {
	std::lock_guard<std::mutex> lock(stateMutex);
	for (uint32_t i = 0; i < IPC::MAX_CONTROLLERS; ++i) {
		controllers[i] = {};
		connectState[i] = IPC::RequestState::Idle;
		pingState[i] = IPC::RequestState::Idle;
	}
}
bool DevlitchInput::UpdateControllers() {
	ResetState();
	return client.func.Send(IPC::PacketType::ListControllers);
}
int DevlitchInput::FindController(IPC::ControllerInfo* controllers, uint32_t id) {
	{
		std::lock_guard<std::mutex> lock(stateMutex);
		for (int i = 0; i < static_cast <int>(IPC::MAX_CONTROLLERS); ++i) {
			if (controllers[i].id == id) { return i; }
		}
		refreshing = true;
	}
	notificationManager.Show(Notification::Type::Warning, "Refreshing list...");
	if (!UpdateControllers()) {
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			refreshing = false;
		}
		notificationManager.Show(Notification::Type::Error, "Failed to refresh controller list.");
		return -1;
	}
	return -1;
}
//============================================================
// Init
//============================================================
void DevlitchInput::init() {
	gui.LoadingOn();
	gui.LoadingText("Starting UI");

	ResetState();
	gui.Resume();

	receiveThread = std::thread(&DevlitchInput::ReceiveLoop, this);
	gui.Wait();
	gui.Resume();
	SDL_Delay(100);
	if (UpdateControllers()) {
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			refreshing = true;
		}
		notificationManager.Show(Notification::Type::Info, "Loading controller list...");
	} else {
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			refreshing = false;
		}
		notificationManager.Show(Notification::Type::Error, "Failed to request controller list.");
	}
}
//============================================================
// Receive Loop
//============================================================
void DevlitchInput::ReceiveLoop() {
	while (gui.isActive()) {
		IPC::PacketHeader header{};
		if (!client.func.ReceiveHeader(header)) {
			if (gui.isActive()) Stop();
			break;
		}
		switch (header.type) {
		//====================================================
		// Success
		//====================================================
		case IPC::PacketType::Success: {
			IPC::SuccessPayload response{};
			if (!client.func.ReceivePayload(response, header.payloadSize)) {
				Stop();
				break;
			}
			HandleSuccess(response);
			break;
		}
		//====================================================
		// Error
		//====================================================
		case IPC::PacketType::Error: {
			IPC::ErrorPayload response{};
			if (!client.func.ReceivePayload(response, header.payloadSize)) {
				Stop();
				break;
			}
			HandleError(response);
			break;
		}
		//====================================================
		// Controller List
		//====================================================
		case IPC::PacketType::ControllerList: {
			IPC::ControllerListPayload response{};
			if (!client.func.ReceivePayload(response, header.payloadSize)) {
				Stop();
				break;
			}
			HandleControllerList(response);
			break;
		}
		//====================================================
		// Controller Connected
		//====================================================
		case IPC::PacketType::ControllerConnected: {
			IPC::ControllerPayload payload{};
			if (!client.func.ReceivePayload(payload, header.payloadSize)) {
				Stop();
				break;
			}
			HandleControllerState(payload.controllerId, ControllerState::Connect);
			break;
		}
		//====================================================
		// Controller Disconnected
		//====================================================
		case IPC::PacketType::ControllerDisconnected: {
			IPC::ControllerPayload payload{};
			if (!client.func.ReceivePayload(payload, header.payloadSize)) {
				Stop();
				break;
			}
			HandleControllerState(payload.controllerId, ControllerState::Disconnect);
			break;
		}
		//====================================================
		// Controller Pinged
		//====================================================
		case IPC::PacketType::ControllerPinged: {
			IPC::ControllerPayload payload{};
			if (!client.func.ReceivePayload(payload, header.payloadSize)) {
				Stop();
				break;
			}
			HandleControllerState(payload.controllerId, ControllerState::Ping);
			break;
		}
		//====================================================
		// Unknown
		//====================================================
		default: {
			if (header.payloadSize > 0) {
				std::vector<uint8_t> payload(header.payloadSize);
				if (!client.func.ReceivePayload(payload.data(), header.payloadSize)) {
					Stop();
				}
			}
			break;
		}
		}

	}
}
//============================================================
// Success Handler
//============================================================
void DevlitchInput::HandleSuccess(const IPC::SuccessPayload& response) {
	switch (response.type) {
	case IPC::SuccessType::StartUI: {
		gui.Pause();
		break;
	}
	default:
		break;
	}
}
//============================================================
// Error Handler
//============================================================
void DevlitchInput::HandleError(const IPC::ErrorPayload& response) {
	switch (response.type) {
	case IPC::ErrorType::ViGEmInit: {
		gui.ShowError("ViGEm init failed");
		gui.Stop();
		break;
	}
	case IPC::ErrorType::SDLInit: {
		gui.ShowError("SDL failed");
		gui.Stop();
		break;
	}
	case IPC::ErrorType::Connect:
	case IPC::ErrorType::Ping: {
		std::string notiMsg;
		uint8_t status = response.errorCode / 100;
		uint8_t controllerId = response.errorCode % 100;
		int index = FindController(controllers, controllerId);
		if (index < 0) break;
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			notiMsg = controllers[index].name;
			if (response.type != IPC::ErrorType::Ping) {
				controllers[index].connected = status;
				connectState[index] = IPC::RequestState::Failed;
				notiMsg += status ? " dis" : " ";
				notiMsg += "connect request failed.";
			} else {
				pingState[index] = IPC::RequestState::Failed;
				notiMsg = notiMsg + " Ping failed.";
			}
			
		}
		notificationManager.Show(Notification::Type::Error, notiMsg);
		break;
	}
	default:
		notificationManager.Show(Notification::Type::Error, "Server returned an error.");
		break;
	}
}
//============================================================
// Controller List
//============================================================
void DevlitchInput::HandleControllerList(const IPC::ControllerListPayload& response) {
	const uint32_t count = std::min(response.count, IPC::MAX_CONTROLLERS);
	ResetState();
	{
		std::lock_guard<std::mutex> lock(stateMutex);
		refreshing = false;
		for (uint32_t i = 0; i < count; ++i) {
			controllers[i] = response.controllers[i];
		}
	}
	notificationManager.Show(Notification::Type::Success, "Controller list updated. " + std::to_string(count) + " device(s) found.");
}
//============================================================
// Controller State Changed
//============================================================
void DevlitchInput::HandleControllerState(uint32_t controllerId, ControllerState type) {
	std::string notiMsg;
	int index = FindController(controllers, controllerId);
	if (index < 0) return;
	{
		std::lock_guard<std::mutex> lock(stateMutex);
		notiMsg = controllers[index].name;
		if (type != ControllerState::Ping) {
			bool connectionState = type == ControllerState::Connect;
			controllers[index].connected = connectionState ? 1: 0;
			connectState[index] = IPC::RequestState::Success;
			notiMsg += connectionState ? " connected." : " disconnected.";
		}
		else {
			pingState[index] = IPC::RequestState::Success;
			notiMsg += " Pinged";
		}
	}
	notificationManager.Show(Notification::Type::Success, notiMsg);
}
//============================================================
// Stop
//============================================================
void DevlitchInput::Stop() {
	client.func.Disconnect();
	if (receiveThread.joinable() && receiveThread.get_id() != std::this_thread::get_id()) receiveThread.join();
}