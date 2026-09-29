#include "ControllerHub.h"

#include "../../DI/nput.h"
#include "../GUI.h"
#include "../UI.h"
#include "../Notification.h"
#include "../Tabs.h"

#include <imgui.h>

void ControllerHub::Render() {
	//========================================================
	// Connected Count
	//========================================================
	int connectedCount = 0;
	bool currentRefreshing = false;
	{
		std::lock_guard<std::mutex> lock(di.stateMutex);
		for (uint32_t i = 0; i < IPC::MAX_CONTROLLERS; ++i) {
			if (di.controllers[i].info.id != 0 && di.controllers[i].info.connected) ++connectedCount;
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
	// Settings
	//========================================================
	ImGui::SetCursorPos(ImVec2(headerRight - 38.0f, 30.0f));

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 settingsPos = ImGui::GetCursorScreenPos();
	ImVec2 settingsSize(32.0f, 32.0f);

	ImGui::InvisibleButton("##Settings", settingsSize);

	if (ImGui::IsItemClicked()) tab.setTab(Tab::Settings);

	bool hovered = ImGui::IsItemHovered();

	ImVec2 center(
		settingsPos.x + settingsSize.x * 0.5f,
		settingsPos.y + settingsSize.y * 0.5f
	);

	//========================================================
	// Settings Gear
	//========================================================
	ImU32 gearColor = hovered ? IM_COL32(220, 225, 235, 255) : IM_COL32(150, 158, 175, 255);

	const float outerRadius = 9.0f;
	const float innerRadius = 6.8f;
	const float holeRadius = 2.7f;

	const int teeth = 8;
	const int pointsPerTooth = 4;

	ImVec2 gearCenter(
		settingsPos.x + settingsSize.x * 0.5f,
		settingsPos.y + settingsSize.y * 0.5f
	);

	ImVector<ImVec2> gearPoints;
	gearPoints.reserve(teeth * pointsPerTooth);

	//========================================================
	// Gear Shape
	//========================================================
	for (int i = 0; i < teeth; ++i) {

		float baseAngle = (2.0f * PI / teeth) * i;

		// inner -> outer -> outer -> inner
		constexpr float toothWidth = 0.30f;

		float angles[4] = {
			baseAngle - toothWidth,
			baseAngle - toothWidth * 0.45f,
			baseAngle + toothWidth * 0.45f,
			baseAngle + toothWidth
		};

		float radii[4] = {
			innerRadius,
			outerRadius,
			outerRadius,
			innerRadius
		};

		for (int j = 0; j < 4; ++j) {
			gearPoints.push_back(ImVec2(
				gearCenter.x + cosf(angles[j]) * radii[j],
				gearCenter.y + sinf(angles[j]) * radii[j]
			));
		}
	}

	// Gear Body
	drawList->AddConvexPolyFilled(
		gearPoints.Data,
		gearPoints.Size,
		gearColor
	);

	//========================================================
	// Center Hole
	//========================================================
	drawList->AddCircleFilled(
		gearCenter,
		holeRadius,
		IM_COL32(30, 34, 45, 255),
		20
	);

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
		ControllerView& controller = di.controllers[i];
		//====================================================
		// Empty slot
		//====================================================
		if (controller.info.id == 0) continue;
		float y = 82.0f + visibleRow * rowHeight;
		++visibleRow;
		ImGui::SetCursorPos(ImVec2(12.0f, y));
		ImGui::PushID(static_cast <int> (controller.info.id));
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
		UI::DrawLED(drawList, ImVec2(rowMin.x + 27.0f, rowMin.y + 30.0f), controller.info.connected != 0);
		//====================================================
		// Controller Name
		//====================================================
		ImGui::SetCursorPos(ImVec2(52.0f, 19.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, controller.info.connected ? UI::Text : UI::Muted);
		ImGui::TextUnformatted(controller.info.name);
		ImGui::PopStyleColor();
		//====================================================
		// Connect Pending
		//====================================================
		bool connectPending = controller.connectState == RequestState::Pending;
		if (connectPending) {
			ImGui::SetCursorPos(ImVec2(52.0f, 40.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, UI::Yellow);
			ImGui::TextUnformatted(controller.info.connected ? "Disconnecting" : "Connecting");
			ImGui::PopStyleColor();
		}
		//====================================================
		// Ping
		//====================================================
		bool pingPending = controller.pingState == RequestState::Pending;
		bool rumbleSwitchPending = controller.rumbleSwitchState == RequestState::Pending;
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 265.0f, 11.0f));
		std::string vibrationId = "Vibration##" + std::to_string(controller.info.id);
		ImVec2 vibrationPopupPos;
		bool pingClicked = UI::VibrationButton(vibrationId.c_str(), controller.info.connected, pingPending, vibrationPopupPos);
		bool rumbleSwitchToggled = UI::RenderVibrationDropdown(
			vibrationId.c_str(),
			controller.info.rumble,
			vibrationPopupPos
		);
		//====================================================
		// Rumble switch Request
		//====================================================
		if (rumbleSwitchToggled && !rumbleSwitchPending) {
			bool canSwitch = false;
			{
				std::lock_guard<std::mutex> lock(di.stateMutex);
				if (controller.rumbleSwitchState != RequestState::Pending) {
					controller.rumbleSwitchState = RequestState::Pending;
					canSwitch = true;
				}
			}
			if (canSwitch) {
				IPC::SwitchRumblePayload payload{};
				payload.controllerId = controller.info.id;
				payload.enabled = !controller.info.rumble;
				if (!client.func.Send(IPC::PacketType::SetRumble, payload)) {
					{
						std::lock_guard<std::mutex> lock(di.stateMutex);
						controller.rumbleSwitchState = RequestState::Failed;
					}
					notificationManager.Show(Notification::Type::Error, "Failed to send switch rumble request.");
				} else {
					notificationManager.Show(Notification::Type::Info, std::string(!controller.info.rumble ? "Enabling rumble " : "Disabling rumble ") + controller.info.name + "...");
				}
			}
		}
		//====================================================
		// Ping Request
		//====================================================
		if (pingClicked && !pingPending) {
			bool canPing = false;
			{
				std::lock_guard<std::mutex> lock(di.stateMutex);
				if (controller.pingState != RequestState::Pending) {
					controller.pingState = RequestState::Pending;
					canPing = true;
				}
			}
			if (canPing) {
				IPC::TestRumblePayload payload{};
				payload.controllerId = controller.info.id;
				payload.largeMotor = 16000;
				payload.smallMotor = 8000;
				payload.durationMs = 869;
				if (!client.func.Send(IPC::PacketType::TestRumble, payload)) {
					{
						std::lock_guard<std::mutex> lock(di.stateMutex);
						controller.pingState = RequestState::Failed;
					}
					notificationManager.Show(Notification::Type::Error, "Failed to send ping request.");
				} else {
					notificationManager.Show(Notification::Type::Info, "Checking " + std::string(controller.info.name) + "...");
				}
			}
		}
		//====================================================
		// Connect / Disconnect
		//====================================================
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 120.0f, 13.0f));
		const char* action;
		if (connectPending) {
			action = controller.info.connected ? "Disconnecting" : "Connecting";
		} else {
			action = controller.info.connected ? "Disconnect" : "Connect";
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
		else if (controller.info.connected) {
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
			bool shouldDisconnect = controller.info.connected != 0;
			{
				std::lock_guard<std::mutex> lock(di.stateMutex);
				if (controller.connectState != RequestState::Pending) {
					controller.connectState = RequestState::Pending;
					canSend = true;
				}
			}
			if (canSend) {
				//================================================
				// Disconnect
				//================================================
				if (shouldDisconnect) {
					notificationManager.Show(Notification::Type::Info, "Disconnecting " + std::string(controller.info.name) + "...");
					IPC::ControllerPayload payload{};
					payload.controllerId = controller.info.id;
					if (!client.func.Send(IPC::PacketType::Disconnect, payload)) {
						std::lock_guard<std::mutex> lock(di.stateMutex);
						controller.connectState = RequestState::Failed;
						notificationManager.Show(Notification::Type::Error, "Failed to send disconnect request.");
					}
				}
				//================================================
				// Connect
				//================================================
				else {
					notificationManager.Show(Notification::Type::Info, "Connecting " + std::string(controller.info.name) + "...");
					IPC::ControllerPayload payload{};
					payload.controllerId = controller.info.id;
					if (!client.func.Send(IPC::PacketType::Connect, payload)) {
						std::lock_guard<std::mutex> lock(di.stateMutex);
						controller.connectState = RequestState::Failed;
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
}