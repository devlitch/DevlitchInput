#include "Notification.h"

#include <algorithm>

//============================================================
// Colors
//============================================================
namespace NotificationUI {
	constexpr ImVec4 Text = ImVec4(0.94f, 0.95f, 0.98f, 1.0f);
	constexpr ImVec4 Muted = ImVec4(0.45f, 0.48f, 0.56f, 1.0f);
	constexpr ImVec4 Blue = ImVec4(0.30f, 0.60f, 1.00f, 1.0f);
	constexpr ImVec4 Green = ImVec4(0.20f, 0.90f, 0.42f, 1.0f);
	constexpr ImVec4 Red = ImVec4(1.00f, 0.25f, 0.30f, 1.0f);
	constexpr ImVec4 Yellow = ImVec4(1.00f, 0.72f, 0.22f, 1.0f);
}
//============================================================
// Show
//============================================================
void NotificationManager::Show(Notification::Type type, const std::string& message, float duration) {
	std::lock_guard<std::mutex> lock(mutex);
	notification.type = type;
	notification.message = message;
	notification.duration = duration;
	notification.timeLeft = duration;
}
//============================================================
// Clear
//============================================================
void NotificationManager::Clear() {
	std::lock_guard<std::mutex> lock(mutex);
	notification = {};
}
//============================================================
// Render
//============================================================
void NotificationManager::Render() {
	Notification::State current;
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (notification.type == Notification::Type::None) return;
		notification.timeLeft -= ImGui::GetIO().DeltaTime;
		if (notification.timeLeft <= 0.0f) {
			notification = {};
			return;
		}
		current = notification;
	}
	//========================================================
	// Colors
	//========================================================
	ImVec4 bg;
	ImVec4 border;
	ImVec4 accent;
	const char* title = "Info";
	switch (current.type) {
	case Notification::Type::Success:
		bg = ImVec4(0.035f, 0.12f, 0.065f, 0.98f);
		border = ImVec4(0.10f, 0.55f, 0.25f, 1.0f);
		accent = NotificationUI::Green;
		title = "Success";
		break;
	case Notification::Type::Error:
		bg = ImVec4(0.15f, 0.035f, 0.045f, 0.98f);
		border = ImVec4(0.65f, 0.10f, 0.15f, 1.0f);
		accent = NotificationUI::Red;
		title = "Error";
		break;
	case Notification::Type::Warning:
		bg = ImVec4(0.15f, 0.11f, 0.035f, 0.98f);
		border = ImVec4(0.65f, 0.45f, 0.08f, 1.0f);
		accent = NotificationUI::Yellow;
		title = "Warning";
		break;
	case Notification::Type::Info:
	default:
		bg = ImVec4(0.035f, 0.075f, 0.14f, 0.98f);
		border = ImVec4(0.10f, 0.35f, 0.70f, 1.0f);
		accent = NotificationUI::Blue;
		title = "Info";
		break;
	}
	//========================================================
	// Viewport
	//========================================================
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	constexpr float width = 380.0f;
	constexpr float minHeight = 78.0f;
	constexpr float margin = 24.0f;
	//========================================================
	// Text Size
	//========================================================
	ImVec2 messageSize = ImGui::CalcTextSize(current.message.c_str(), nullptr, false, width - 40.0f);
	float height = std::max(minHeight, 58.0f + messageSize.y);
	height = std::min(height, 150.0f);
	//========================================================
	// Position
	//========================================================
	ImVec2 pos(viewport->Pos.x + viewport->Size.x - width - margin, viewport->Pos.y + viewport->Size.y - height - margin);
	ImVec2 max(pos.x + width, pos.y + height);
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	//========================================================
	// Shadow
	//========================================================
	drawList->AddRectFilled(ImVec2(pos.x + 4.0f, pos.y + 5.0f), ImVec2(max.x + 4.0f, max.y + 5.0f), IM_COL32(0, 0, 0, 90), 12.0f);
	//========================================================
	// Background
	//========================================================
	drawList->AddRectFilled(pos, max, ImGui::ColorConvertFloat4ToU32(bg), 12.0f);
	//========================================================
	// Border
	//========================================================
	drawList->AddRect(pos, max, ImGui::ColorConvertFloat4ToU32(border), 12.0f, 0, 1.0f);
	//========================================================
	// Title
	//========================================================
	drawList->AddText(ImVec2(pos.x + 20.0f, pos.y + 12.0f), ImGui::ColorConvertFloat4ToU32(accent), title);
	//========================================================
	// Message
	//========================================================
	drawList->AddText(ImVec2(pos.x + 20.0f, pos.y + 36.0f), ImGui::ColorConvertFloat4ToU32(NotificationUI::Text), current.message.c_str());
	//========================================================
	// Progress
	//========================================================
	float progress = 0.0f;
	if (current.duration > 0.0f) {
		progress = current.timeLeft / current.duration;
	}
	progress = std::clamp(progress, 0.0f, 1.0f);
	constexpr float progressHeight = 3.0f;
	drawList->AddRectFilled(ImVec2(pos.x + 6.0f, max.y - progressHeight), ImVec2(pos.x + 6.0f + width * progress, max.y), ImGui::ColorConvertFloat4ToU32(accent), 2.0f);
}