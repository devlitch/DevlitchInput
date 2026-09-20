#include "ControllerHub.h"

#include "../../DI/nput.h"
#include "../GUI.h"
#include "../UI.h"
#include "../Notification.h"

#include <imgui.h>

void ControllerHub::init() {
	gui.SetRender([&]() {
		Render();
	});
	gui.LoadingOff();
	gui.Wait();
}

void ControllerHub::Render() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::Begin("Controller Manager", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	//========================================================
	// Connected Count
	//========================================================
	int connectedCount = 0;
	bool currentRefreshing = false;
	{
		std::lock_guard<std::mutex> lock(di.stateMutex);
		for (uint32_t i = 0; i < IPC::MAX_CONTROLLERS; ++i) {
			if (di.controllers[i].id != 0 && di.controllers[i].connected) {
				++connectedCount;
			}
		}
		currentRefreshing = di.refreshing;
	}
	//========================================================
	// Header
	//========================================================
	ImGui::SetCursorPos(ImVec2(42.0f, 36.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, UI::Text);
	ImGui::Text("DevlitchInput");
	ImGui::PopStyleColor();
	//========================================================
	// Online Counter
	//========================================================
	float headerRight = ImGui::GetWindowWidth() - 42.0f;
	ImGui::SetCursorPos(ImVec2(headerRight - 170.0f, 36.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, connectedCount > 0 ? UI::Green : UI::Red);
	ImGui::Text("%d", connectedCount);
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, UI::Muted);
	ImGui::Text("/ %d online", IPC::MAX_CONTROLLERS);
	ImGui::PopStyleColor();
	//========================================================
	// Controller Card
	//========================================================
	ImGui::SetCursorPos(ImVec2(32.0f, 110.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, UI::Card);
	ImGui::BeginChild("##ControllerCard", ImVec2(ImGui::GetWindowWidth() - 64.0f, ImGui::GetWindowHeight() - 200.0f), true);
	//========================================================
	// Card Header
	//========================================================
	ImGui::SetCursorPos(ImVec2(22.0f, 18.0f));
	ImGui::Text("Connected devices");
	ImGui::SetCursorPos(ImVec2(22.0f, 46.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, UI::Muted);
	ImGui::Text("Manage and monitor connected devices");
	ImGui::PopStyleColor();
	//========================================================
	// Refresh
	//========================================================
	const float cardWidth = ImGui::GetWindowWidth();
	ImGui::SetCursorPos(ImVec2(cardWidth - 62.0f, 22.0f));
	if (UI::RefreshButton("RefreshButton", currentRefreshing)) {
		bool canRefresh = false;
		{
			std::lock_guard<std::mutex> lock(di.stateMutex);
			if (!di.refreshing) {
				di.refreshing = true;
				canRefresh = true;
			}
		}
		if (canRefresh) {
			notificationManager.Show(Notification::Type::Info, "Refreshing controller list...");
			if (!di.UpdateControllers()) {
				std::lock_guard<std::mutex> lock(di.stateMutex);
				di.refreshing = false;
				notificationManager.Show(Notification::Type::Error, "Failed to send refresh request.");
			}
		}
	}
	//========================================================
	// Controller Rows
	//========================================================
	constexpr float rowHeight = 62.0f;
	int visibleRow = 0;
	for (uint32_t i = 0; i < IPC::MAX_CONTROLLERS; ++i) {
		IPC::ControllerInfo controller{};
		IPC::RequestState currentConnectState = IPC::RequestState::Idle;
		IPC::RequestState currentPingState = IPC::RequestState::Idle;
		{
			std::lock_guard<std::mutex> lock(di.stateMutex);
			controller = di.controllers[i];
			currentConnectState = di.connectState[i];
			currentPingState = di.pingState[i];
		}
		//====================================================
		// Empty slot
		//====================================================
		if (controller.id == 0) continue;
		float y = 82.0f + visibleRow * rowHeight;
		++visibleRow;
		ImGui::SetCursorPos(ImVec2(12.0f, y));
		ImGui::PushID(static_cast <int> (controller.id));
		//====================================================
		// Row Background
		//====================================================
		ImGui::PushStyleColor(ImGuiCol_ChildBg, visibleRow % 2 == 0 ? ImVec4(0.055f, 0.060f, 0.078f, 1.0f) : ImVec4(0.048f, 0.053f, 0.068f, 1.0f));
		ImGui::BeginChild("##Row", ImVec2(ImGui::GetWindowWidth() - 24.0f, rowHeight - 5.0f), false);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 rowMin = ImGui::GetWindowPos();
		ImVec2 rowMax(rowMin.x + ImGui::GetWindowWidth(), rowMin.y + ImGui::GetWindowHeight());
		//====================================================
		// Bottom line
		//====================================================
		drawList->AddLine(ImVec2(rowMin.x + 20.0f, rowMax.y - 1.0f), ImVec2(rowMax.x - 20.0f, rowMax.y - 1.0f), IM_COL32(255, 255, 255, 5));
		//====================================================
		// LED
		//====================================================
		UI::DrawLED(drawList, ImVec2(rowMin.x + 27.0f, rowMin.y + 30.0f), controller.connected != 0);
		//====================================================
		// Controller Name
		//====================================================
		ImGui::SetCursorPos(ImVec2(52.0f, 19.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, controller.connected ? UI::Text : UI::Muted);
		ImGui::TextUnformatted(controller.name);
		ImGui::PopStyleColor();
		//====================================================
		// Connect Pending
		//====================================================
		bool connectPending = currentConnectState == IPC::RequestState::Pending;
		if (connectPending) {
			ImGui::SetCursorPos(ImVec2(52.0f, 40.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, UI::Yellow);
			ImGui::TextUnformatted(controller.connected ? "Disconnecting" : "Connecting");
			ImGui::PopStyleColor();
		}
		//====================================================
		// Ping
		//====================================================
		bool pingPending = currentPingState == IPC::RequestState::Pending;
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 265.0f, 11.0f));
		std::string bellId = "Bell##" + std::to_string(controller.id);
		bool pingClicked = UI::BellButton(bellId.c_str(), true, pingPending);
		//====================================================
		// Ping Request
		//====================================================
		if (pingClicked && !pingPending) {
			bool canPing = false;
			{
				std::lock_guard<std::mutex> lock(di.stateMutex);
				if (di.pingState[i] != IPC::RequestState::Pending) {
					di.pingState[i] = IPC::RequestState::Pending;
					canPing = true;
				}
			}
			if (canPing) {
				IPC::TestRumblePayload payload{};
				payload.controllerId = controller.id;
				payload.largeMotor = 16000;
				payload.smallMotor = 8000;
				payload.durationMs = 869;
				if (!client.func.Send(IPC::PacketType::TestRumble, payload)) {
					{
						std::lock_guard<std::mutex> lock(di.stateMutex);
						di.pingState[i] = IPC::RequestState::Failed;
					}
					notificationManager.Show(Notification::Type::Error, "Failed to send ping request.");
				} else {
					notificationManager.Show(Notification::Type::Info, "Checking " + std::string(controller.name) + "...");
				}
			}
		}
		//====================================================
		// Connect / Disconnect
		//====================================================
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 120.0f, 13.0f));
		const char* action;
		if (connectPending) {
			action = controller.connected ? "Disconnecting" : "Connecting";
		} else {
			action = controller.connected ? "Disconnect" : "Connect";
		}
		bool actionClicked = false;
		//====================================================
		// Pending
		//====================================================
		if (connectPending) {
			UI::ModernButton(action, ImVec2(105.0f, 36.0f), ImVec4(0.08f, 0.10f, 0.14f, 1.0f), ImVec4(0.08f, 0.10f, 0.14f, 1.0f), ImVec4(0.08f, 0.10f, 0.14f, 1.0f));
		}
		//====================================================
		// Disconnect
		//====================================================
		else if (controller.connected) {
			actionClicked = UI::ModernButton(action, ImVec2(105.0f, 36.0f), ImVec4(0.16f, 0.055f, 0.065f, 1.0f), ImVec4(0.30f, 0.07f, 0.08f, 1.0f), ImVec4(0.20f, 0.05f, 0.06f, 1.0f));
		}
		//====================================================
		// Connect
		//====================================================
		else {
			actionClicked = UI::ModernButton(action, ImVec2(105.0f, 36.0f), ImVec4(0.05f, 0.18f, 0.10f, 1.0f), ImVec4(0.07f, 0.32f, 0.17f, 1.0f), ImVec4(0.05f, 0.22f, 0.12f, 1.0f));
		}
		//====================================================
		// Connect / Disconnect Request
		//====================================================
		if (actionClicked && !connectPending) {
			bool canSend = false;
			bool shouldDisconnect = controller.connected != 0;
			{
				std::lock_guard<std::mutex> lock(di.stateMutex);
				if (di.connectState[i] != IPC::RequestState::Pending) {
					di.connectState[i] = IPC::RequestState::Pending;
					canSend = true;
				}
			}
			if (canSend) {
				//================================================
				// Disconnect
				//================================================
				if (shouldDisconnect) {
					notificationManager.Show(Notification::Type::Info, "Disconnecting " + std::string(controller.name) + "...");
					IPC::ControllerPayload payload{};
					payload.controllerId = controller.id;
					if (!client.func.Send(IPC::PacketType::Disconnect, payload)) {
						std::lock_guard<std::mutex> lock(di.stateMutex);
						di.connectState[i] = IPC::RequestState::Failed;
						notificationManager.Show(Notification::Type::Error, "Failed to send disconnect request.");
					}
				}
				//================================================
				// Connect
				//================================================
				else {
					notificationManager.Show(Notification::Type::Info, "Connecting " + std::string(controller.name) + "...");
					IPC::ControllerPayload payload{};
					payload.controllerId = controller.id;
					if (!client.func.Send(IPC::PacketType::Connect, payload)) {
						std::lock_guard<std::mutex> lock(di.stateMutex);
						di.connectState[i] = IPC::RequestState::Failed;
						notificationManager.Show(Notification::Type::Error, "Failed to send connect request.");
					}
				}
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopID();
	}
	//========================================================
	// End Card
	//========================================================
	ImGui::EndChild();
	ImGui::PopStyleColor();
	//========================================================
	// Footer
	//========================================================
	const char* leftText = "https://github.com/devlitch/DevlitchInput";
	const char* rightText = "v1.5";
	float rightWidth = ImGui::CalcTextSize(rightText).x;
	ImGui::PushStyleColor(ImGuiCol_Text, UI::Dim);
	ImGui::SetCursorPosX(7.0f);
	ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 32.0f);
	ImGui::TextUnformatted(leftText);
	ImGui::SameLine(ImGui::GetWindowWidth() - rightWidth - 7.0f);
	ImGui::TextUnformatted(rightText);
	ImGui::PopStyleColor();
	ImGui::End();
}