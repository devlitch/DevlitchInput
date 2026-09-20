#include "Loading.h"

#include <cmath>
#include <cstring>
#include <atomic>

#include "imgui.h"

namespace Loading {
	// ============================================================
	// State
	// ============================================================
	static std::atomic<bool> active{ true };
	static float animationTime = 0.0f;
	static char statusText[256] = "Loading";
	// ============================================================
	// Colors
	// ============================================================
	namespace Colors {
		constexpr ImVec4 Text = ImVec4(0.94f, 0.95f, 0.98f, 1.0f);
		constexpr ImVec4 Muted = ImVec4(0.45f, 0.48f, 0.56f, 1.0f);
		constexpr ImVec4 Dim = ImVec4(0.28f, 0.30f, 0.36f, 1.0f);
		constexpr ImVec4 Cyan = ImVec4(0.10f, 0.82f, 1.00f, 1.0f);
	}
	// ============================================================
	// On
	// ============================================================
	void On() {
		active.store(true);
		active.notify_all();
		animationTime = 0.0f;
		strncpy_s(statusText, sizeof(statusText),"Loading", _TRUNCATE);

		statusText[sizeof(statusText) - 1] = '\0';
	}
	// ============================================================
	// Off
	// ============================================================
	void Off() {
		active.store(false);
		active.notify_all();
	}
	// ============================================================
	// IsActive
	// ============================================================
	bool IsActive() {
		return active.load();
	}
	// ============================================================
	// Text
	// ============================================================
	void Text(const char* reason) {
		strncpy_s(statusText, sizeof(statusText), reason, _TRUNCATE);
		statusText[sizeof(statusText) - 1] = '\0';
	}
	// ============================================================
	// Update
	// ============================================================
	void Update(float deltaTime) {
		if (!IsActive()) {
			return;
		}
		animationTime += deltaTime;
	}
	// ============================================================
	// Spinner
	// ============================================================
	static void DrawSpinner(ImDrawList* drawList, ImVec2 center, float radius) {
		constexpr int segments = 32;
		constexpr float TWO_PI = 6.28318530718f;
		float rotation = animationTime * 4.0f;
		for (int i = 0; i < segments; ++i) {
			float t = static_cast <float>(i) / static_cast <float>(segments);
			float angle = rotation + t * TWO_PI;
			float alpha = 0.08f + t * 0.92f;
			ImU32 color = IM_COL32(80, 170, 255, static_cast <int>(alpha * 255.0f));
			ImVec2 p1(center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius);
			ImVec2 p2(center.x + std::cos(angle) * (radius + 2.0f), center.y + std::sin(angle) * (radius + 2.0f));
			drawList->AddLine(p1, p2, color, 2.5f);
		}
	}
	// ============================================================
	// Draw
	// ============================================================
	void Draw() {
		if (!IsActive()) {
			return;
		}
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::Begin("##Loading", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		const float width = ImGui::GetWindowWidth();
		const float height = ImGui::GetWindowHeight();
		const float centerX = width * 0.5f;
		const float centerY = height * 0.5f;
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		// ========================================================
		// Spinner
		// ========================================================
		DrawSpinner(drawList, ImVec2(centerX, centerY - 38.0f), 15.0f);
		// ========================================================
		// Reason
		// ========================================================
		ImVec2 reasonSize = ImGui::CalcTextSize(statusText);
		ImGui::SetCursorPos(ImVec2(centerX - reasonSize.x * 0.5f, centerY - 3.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, Colors::Muted);
		ImGui::TextUnformatted(statusText);
		ImGui::PopStyleColor();
		// ========================================================
		// Animated dots
		// ========================================================
		int dots = static_cast <int> (animationTime * 2.5f) % 4;
		char dotsText[4]{};
		for (int i = 0; i < dots; ++i) {
			dotsText[i] = '.';
		}
		ImGui::SameLine(0.0f, 1.5f);
		ImGui::PushStyleColor(ImGuiCol_Text, Colors::Cyan);
		ImGui::TextUnformatted(dotsText);
		ImGui::PopStyleColor();
		// ========================================================
		// Brand
		// ========================================================
		const char* brand = "DEVLITCHINPUT";
		ImVec2 brandSize = ImGui::CalcTextSize(brand);
		ImGui::SetCursorPos(ImVec2(centerX - brandSize.x * 0.5f, height - 48.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, Colors::Dim);
		ImGui::TextUnformatted(brand);
		ImGui::PopStyleColor();
		ImGui::End();
	}
}