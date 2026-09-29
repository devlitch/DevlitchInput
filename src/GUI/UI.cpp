#include "UI.h"

#include <cmath>

namespace UI {
	const ImVec4 Bg = ImVec4(0.025f, 0.028f, 0.038f, 1.0f);
	const ImVec4 Card = ImVec4(0.045f, 0.050f, 0.065f, 1.0f);
	const ImVec4 CardHover = ImVec4(0.060f, 0.067f, 0.085f, 1.0f);

	const ImVec4 Text = ImVec4(0.94f, 0.95f, 0.98f, 1.0f);
	const ImVec4 Muted = ImVec4(0.45f, 0.48f, 0.56f, 1.0f);
	const ImVec4 Dim = ImVec4(0.28f, 0.30f, 0.36f, 1.0f);

	const ImVec4 Blue = ImVec4(0.30f, 0.60f, 1.00f, 1.0f);
	const ImVec4 BlueHover = ImVec4(0.40f, 0.68f, 1.00f, 1.0f);
	const ImVec4 Cyan = ImVec4(0.10f, 0.82f, 1.00f, 1.0f);

	const ImVec4 Green = ImVec4(0.20f, 0.90f, 0.42f, 1.0f);
	const ImVec4 Red = ImVec4(1.00f, 0.25f, 0.30f, 1.0f);
	const ImVec4 Yellow = ImVec4(1.00f, 0.72f, 0.22f, 1.0f);

	const ImVec4 Border = ImVec4(0.11f, 0.12f, 0.15f, 1.0f);
	const ImVec4 BorderHover = ImVec4(0.18f, 0.20f, 0.25f, 1.0f);

	void UI::SetupStyle() {
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(0, 0);
		style.FramePadding = ImVec2(12.0f, 8.0f);
		style.ItemSpacing = ImVec2(10.0f, 8.0f);
		style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
		style.ChildBorderSize = 1.0f;
		style.WindowBorderSize = 0.0f;
		style.ChildRounding = 14.0f;
		style.FrameRounding = 9.0f;
		style.PopupRounding = 10.0f;
		style.ScrollbarRounding = 8.0f;
		ImVec4* c = style.Colors;
		c[ImGuiCol_Text] = Text;
		c[ImGuiCol_TextDisabled] = Dim;
		c[ImGuiCol_WindowBg] = Bg;
		c[ImGuiCol_ChildBg] = Card;
		c[ImGuiCol_PopupBg] = Card;
		c[ImGuiCol_Border] = Border;
		c[ImGuiCol_FrameBg] = Card;
		c[ImGuiCol_FrameBgHovered] = CardHover;
		c[ImGuiCol_FrameBgActive] = CardHover;
		c[ImGuiCol_Button] = Card;
		c[ImGuiCol_ButtonHovered] = CardHover;
		c[ImGuiCol_ButtonActive] = CardHover;
		c[ImGuiCol_Header] = Card;
		c[ImGuiCol_HeaderHovered] = CardHover;
		c[ImGuiCol_HeaderActive] = CardHover;
		c[ImGuiCol_Separator] = Border;
		c[ImGuiCol_SeparatorHovered] = BorderHover;
		c[ImGuiCol_SeparatorActive] = Blue;
		c[ImGuiCol_ScrollbarBg] = Bg;
		c[ImGuiCol_ScrollbarGrab] = BorderHover;
		c[ImGuiCol_ScrollbarGrabHovered] = Blue;
		c[ImGuiCol_ScrollbarGrabActive] = BlueHover;
	}
	void UI::SetupFont() {
		ImGuiIO& io = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 18.0f);
		if(!font) io.Fonts->AddFontDefault();
	}

	bool Button(const char* label, bool active, ImVec2 size) {
		ImVec4 normal = active ? ImVec4(0.08f, 0.16f, 0.27f, 1.0f) : ImVec4(0.045f, 0.050f, 0.065f, 1.0f);
		ImVec4 hover = ImVec4(0.10f, 0.20f, 0.32f, 1.0f);
		ImVec4 activeColor = ImVec4(0.12f, 0.23f, 0.36f, 1.0f);
		return ModernButton(label, size, normal, hover, activeColor);
	}
	bool ModernButton(const char* label, ImVec2 size, ImVec4 normal, ImVec4 hover, ImVec4 active, bool center) {
		ImGui::PushStyleColor(ImGuiCol_Button, normal);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);
		bool result;
		if (center) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));

			result = ImGui::Button(label, size);

			ImGui::PopStyleColor();

			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();

			ImVec2 textSize = ImGui::CalcTextSize(label);

			float x = min.x + (max.x - min.x - textSize.x) * 0.5f;
			float y = min.y + (max.y - min.y - textSize.y) * 0.5f;

			y -= 2.0f;
			ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(x, y), ImGui::GetColorU32(ImGuiCol_Text), label);
		} else {
			result = ImGui::Button(label, size);
		}
		ImGui::PopStyleColor(3);
		return result;
	}
	void DrawLED(ImDrawList* drawList, ImVec2 center, bool connected) {
		if(connected) {
			drawList->AddCircleFilled(center, 11.0f, IM_COL32(50, 255, 100, 12));
			drawList->AddCircleFilled(center, 8.0f, IM_COL32(50, 255, 100, 30));
			drawList->AddCircleFilled(center, 5.0f, IM_COL32(55, 255, 105, 255));
			drawList->AddCircleFilled(ImVec2(center.x - 1.5f, center.y - 1.5f), 1.5f, IM_COL32(220, 255, 230, 240));
		} else {
			drawList->AddCircleFilled(center, 8.0f, IM_COL32(255, 50, 60, 20));
			drawList->AddCircleFilled(center, 5.0f, IM_COL32(255, 65, 70, 255));
		}
	}
	static void DrawVibrationIcon(ImDrawList* drawList, ImVec2 center, ImU32 color, float scale = 1.0f) {
		const float width = 18.0f * scale;
		const float height = 14.0f * scale;
		const float left = center.x - width * 0.5f;
		const float right = center.x + width * 0.5f;
		const float top = center.y - height * 0.5f;
		const float bottom = center.y + height * 0.5f;
		drawList->PathClear();
		// Left vibration wave
		drawList->PathLineTo(ImVec2(left, center.y));
		drawList->PathLineTo(ImVec2(left + width * 0.12f, top));
		drawList->PathLineTo(ImVec2(left + width * 0.24f, bottom));
		// Main wave
		drawList->PathLineTo(ImVec2(center.x - width * 0.12f, top));
		drawList->PathLineTo(ImVec2(center.x, bottom));
		drawList->PathLineTo(ImVec2(center.x + width * 0.12f, top));
		drawList->PathLineTo(ImVec2(center.x + width * 0.24f, bottom));
		// Right vibration wave
		drawList->PathLineTo(ImVec2(right, center.y));
		drawList->PathStroke(color, false, 2.0f * scale);
	}
	bool VibrationButton(const char* id, bool active, bool pending, ImVec2& popupPos) {
		ImGui::PushID(id);
		constexpr float buttonSize = 40.0f;
		constexpr float arrowWidth = 22.0f;
		constexpr float spacing = 2.0f;
		ImVec2 pos = ImGui::GetCursorScreenPos();
		popupPos = ImVec2(pos.x, pos.y + buttonSize + 4.0f);
		//================================================
		// Vibration button
		//================================================
		bool vibrationClicked = ImGui::InvisibleButton("##vibration", ImVec2(buttonSize, buttonSize));
		bool vibrationHovered = ImGui::IsItemHovered();
		if (pending) vibrationClicked = false;
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec4 bg;
		if (pending) {
			bg = ImVec4(0.05f, 0.055f, 0.065f, 1.0f);
		} else {
			bg = vibrationHovered ? ImVec4(0.10f, 0.16f, 0.25f, 1.0f) : ImVec4(0.07f, 0.10f, 0.16f, 1.0f);
		}
		drawList->AddRectFilled(pos, ImVec2(pos.x + buttonSize, pos.y + buttonSize), ImGui::ColorConvertFloat4ToU32(bg), 10.0f);
		ImVec2 center(pos.x + buttonSize * 0.5f, pos.y + buttonSize * 0.5f);
		if (pending) {
			DrawVibrationIcon(drawList, center, IM_COL32(120, 130, 145, 100));
		} else {
			ImU32 iconColor = vibrationHovered ? IM_COL32(100, 175, 255, 255) : IM_COL32(120, 155, 205, 255);
			DrawVibrationIcon(drawList, center, iconColor);
		}
		//================================================
		// Dropdown button
		//================================================
		ImGui::SameLine(0.0f, spacing);
		ImVec2 arrowPos = ImGui::GetCursorScreenPos();
		bool arrowClicked = ImGui::InvisibleButton("##vibration_dropdown", ImVec2(arrowWidth, buttonSize));
		bool arrowHovered = ImGui::IsItemHovered();
		if (arrowClicked && active) {
			ImGui::OpenPopup("##VibrationDropdown");
		}
		//================================================
		// Arrow background
		//================================================
		ImVec4 arrowBg;
		if (!active) {
			arrowBg = ImVec4(0.05f, 0.055f, 0.065f, 1.0f);
		} else if (arrowHovered) {
			arrowBg = ImVec4(0.10f, 0.16f, 0.25f, 1.0f);
		} else {
			arrowBg = ImVec4(0.07f, 0.10f, 0.16f, 1.0f);
		}
		drawList->AddRectFilled(arrowPos, ImVec2(arrowPos.x + arrowWidth, arrowPos.y + buttonSize), ImGui::ColorConvertFloat4ToU32(arrowBg), 10.0f);
		//================================================
		// Separator
		//================================================
		drawList->AddLine(ImVec2(arrowPos.x, arrowPos.y + 9.0f), ImVec2(arrowPos.x, arrowPos.y + buttonSize - 9.0f), IM_COL32(55, 65, 80, 180), 1.0f);
		//================================================
		// Chevron
		//================================================
		ImVec2 arrowCenter(arrowPos.x + arrowWidth * 0.5f, arrowPos.y + buttonSize * 0.5f);
		ImU32 arrowColor = active ? IM_COL32(130, 155, 190, 255) : IM_COL32(90, 95, 105, 100);
		constexpr float arrowSize = 4.0f;
		drawList->AddLine(ImVec2(arrowCenter.x - arrowSize, arrowCenter.y - 2.0f), ImVec2(arrowCenter.x, arrowCenter.y + 2.0f), arrowColor, 1.5f);
		drawList->AddLine(ImVec2(arrowCenter.x, arrowCenter.y + 2.0f), ImVec2(arrowCenter.x + arrowSize, arrowCenter.y - 2.0f), arrowColor, 1.5f);
		ImGui::PopID();
		return vibrationClicked;
	}
	bool RenderVibrationDropdown(const char* id, bool& rumble, const ImVec2& popupPos) {
		ImGui::PushID(id);
		ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(310.0f, 92.0f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.045f, 0.055f, 0.075f, 0.98f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.12f, 0.18f, 0.28f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 12.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 14.0f));
		bool toggleClicked = false;
		if (ImGui::BeginPopup("##VibrationDropdown")) {
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImVec2 windowPos = ImGui::GetWindowPos();
			//================================================
			// Icon
			//================================================
			DrawVibrationIcon(drawList, ImVec2(windowPos.x + 20.0f, windowPos.y + 22.0f), IM_COL32(100, 175, 255, 255), 0.85f);
			//================================================
			// Title
			//================================================
			ImGui::SetCursorPos(ImVec2(42.0f, 12.0f));
			ImGui::TextColored(ImVec4(0.88f, 0.91f, 0.97f, 1.0f), "Vibration / Rumble");
			//================================================
			// Toggle
			//================================================
			constexpr float toggleWidth = 38.0f;
			constexpr float toggleHeight = 20.0f;
			ImVec2 togglePos(windowPos.x + 310.0f - toggleWidth - 16.0f, windowPos.y + 14.0f);
			ImGui::SetCursorScreenPos(togglePos);
			toggleClicked = ImGui::InvisibleButton("##toggle", ImVec2(toggleWidth, toggleHeight));
			bool hovered = ImGui::IsItemHovered();
			ImVec4 toggleBg = rumble ? ImVec4(0.15f, 0.48f, 0.95f, 1.0f) : ImVec4(0.16f, 0.18f, 0.22f, 1.0f);
			if (hovered) {
				toggleBg.x += 0.03f;
				toggleBg.y += 0.03f;
				toggleBg.z += 0.03f;
			}
			drawList->AddRectFilled(togglePos, ImVec2(togglePos.x + toggleWidth, togglePos.y + toggleHeight), ImGui::ColorConvertFloat4ToU32(toggleBg), toggleHeight * 0.5f);
			constexpr float knobSize = 16.0f;
			float knobX = rumble ? togglePos.x + toggleWidth - knobSize - 2.0f : togglePos.x + 2.0f;
			drawList->AddCircleFilled(ImVec2(knobX + knobSize * 0.5f, togglePos.y + toggleHeight * 0.5f), knobSize * 0.5f, IM_COL32(245, 248, 255, 255));
			//================================================
			// Description
			//================================================
			ImGui::SetCursorPos(ImVec2(42.0f, 39.0f));
			ImGui::TextColored(ImVec4(0.42f, 0.47f, 0.57f, 1.0f), "Enable vibration for this device.");
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(2);
		ImGui::PopID();
		return toggleClicked;
	}
	void DrawRefreshIcon(ImDrawList* drawList, ImVec2 center, ImU32 color, float radius, float rotation) {
		const float thickness = 2.0f;
		auto RotatePoint = [&](ImVec2 p) -> ImVec2 {
			const float c = cosf(rotation);
			const float s = sinf(rotation);
			float x = p.x - center.x;
			float y = p.y - center.y;
			return ImVec2(center.x + x * c - y * s, center.y + x * s + y * c);
		};
		drawList->PathClear();
		drawList->PathArcTo(center, radius, -0.75f + rotation, 4.8f + rotation, 24);
		drawList->PathStroke(color, thickness);
		ImVec2 arrowTip(center.x + radius * 0.82f, center.y - radius * 0.56f);
		ImVec2 arrowA(arrowTip.x - 5.0f, arrowTip.y - 1.0f);
		ImVec2 arrowB(arrowTip.x - 1.0f, arrowTip.y + 4.0f);
		arrowTip = RotatePoint(arrowTip);
		arrowA = RotatePoint(arrowA);
		arrowB = RotatePoint(arrowB);
		drawList->AddTriangleFilled(arrowTip, arrowA, arrowB, color);
	}
	bool RefreshButton(const char* id, bool pending) {
		ImGui::PushID(id);
		constexpr float size = 40.0f;
		ImVec2 pos = ImGui::GetCursorScreenPos();
		bool clicked = ImGui::InvisibleButton("##refresh", ImVec2(size, size));
		bool hovered = ImGui::IsItemHovered();
		if (pending) clicked = false;
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec4 bg;
		if (pending) {
			bg = ImVec4(0.055f, 0.065f, 0.090f, 1.0f);
		} else {
			bg = hovered ? ImVec4(0.10f, 0.17f, 0.27f, 1.0f) : ImVec4(0.07f, 0.11f, 0.18f, 1.0f);
		}
		drawList->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), ImGui::ColorConvertFloat4ToU32(bg), 9.0f);
		ImVec2 center(pos.x + size * 0.5f, pos.y + size * 0.5f);
		static float rotation = 0.0f;
		if (pending) {
			rotation += ImGui::GetIO().DeltaTime * 7.0f;
			constexpr float TWO_PI = 6.28318530718f;
			if (rotation > TWO_PI) rotation -= TWO_PI;
		}
		ImU32 color;
		if (pending) {
			color = IM_COL32(100, 175, 255, 255);
		} else if (hovered) {
			color = IM_COL32(100, 175, 255, 255);
		} else {
			color = IM_COL32(120, 155, 205, 255);
		}
		DrawRefreshIcon(drawList, center, color, 7.0f, rotation);
		if (hovered && !pending) {
			drawList->AddRect(pos, ImVec2(pos.x + size, pos.y + size), IM_COL32(70, 130, 220, 100), 9.0f, 1.0f);
		}
		ImGui::PopID();
		return clicked;
	}
	bool ModernCheckbox(const char* label, bool* value) {
		ImGui::PushID(label);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		float size = 18.0f;
		bool clicked = ImGui::InvisibleButton("##Checkbox", ImVec2(size, size));
		if (clicked)*value = !*value;
		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImU32 bgColor = *value ? IM_COL32(70, 180, 110, 255) : IM_COL32(35, 40, 52, 255);
		ImU32 borderColor = *value ? IM_COL32(90, 210, 130, 255) : IM_COL32(75, 82, 98, 255);
		draw->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), bgColor, 5.0f);
		draw->AddRect(pos, ImVec2(pos.x + size, pos.y + size), borderColor, 5.0f, 1.2f);
		if (*value) {
			draw->AddLine(ImVec2(pos.x + 4.0f, pos.y + 9.0f), ImVec2(pos.x + 8.0f, pos.y + 13.0f), IM_COL32(255, 255, 255, 255), 2.0f);
			draw->AddLine(ImVec2(pos.x + 8.0f, pos.y + 13.0f), ImVec2(pos.x + 15.0f, pos.y + 5.0f), IM_COL32(255, 255, 255, 255), 2.0f);
		}
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::TextUnformatted(label);
		ImGui::PopID();
		return clicked;
	}

	void UI::init() {
		SetupStyle();
		SetupFont();
	}

};