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
	bool ModernButton(const char* label, ImVec2 size, ImVec4 normal, ImVec4 hover, ImVec4 active) {
		ImGui::PushStyleColor(ImGuiCol_Button, normal);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);
		bool result = ImGui::Button(label, size);
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
	static void DrawBellIcon(ImDrawList* drawList, ImVec2 center, ImU32 color, float scale = 1.0f) {
		const float w = 13.0f* scale;
		const float h = 15.0f* scale;
		ImVec2 top(center.x, center.y - h* 0.45f);
		ImVec2 left(center.x - w* 0.50f, center.y + h* 0.25f);
		ImVec2 right(center.x + w* 0.50f, center.y + h* 0.25f);
		drawList->PathClear();
		drawList->PathLineTo(top);
		drawList->PathBezierCubicCurveTo(ImVec2(center.x - w* 0.45f, center.y - h* 0.20f), ImVec2(center.x - w* 0.50f, center.y + h* 0.05f), left);
		drawList->PathLineTo(right);
		drawList->PathBezierCubicCurveTo(ImVec2(center.x + w* 0.50f, center.y + h* 0.05f), ImVec2(center.x + w* 0.45f, center.y - h* 0.20f), top);
		drawList->PathFillConvex(color);
		drawList->AddRectFilled(ImVec2(center.x - w* 0.62f, center.y + h* 0.27f), ImVec2(center.x + w* 0.62f, center.y + h* 0.38f), color, 3.0f);
		drawList->AddCircleFilled(ImVec2(center.x, center.y + h* 0.48f), 2.0f* scale, color);
	}
	bool BellButton(const char* id, bool enabled, bool pending) {
		ImGui::PushID(id);
		constexpr float size = 40.0f;
		ImVec2 pos = ImGui::GetCursorScreenPos();
		bool clicked = ImGui::InvisibleButton("##bell", ImVec2(size, size));
		bool hovered = ImGui::IsItemHovered();
		if(!enabled || pending) clicked = false;
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 center(pos.x + size* 0.5f, pos.y + size* 0.5f);
		ImVec4 bg;
		if(!enabled || pending) {
			bg = ImVec4(0.05f, 0.055f, 0.065f, 1.0f);
		} else {
			bg = hovered ? ImVec4(0.10f, 0.16f, 0.25f, 1.0f) : ImVec4(0.07f, 0.10f, 0.16f, 1.0f);
		}
		drawList->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), ImGui::ColorConvertFloat4ToU32(bg), 10.0f);
		if(pending) {
			DrawBellIcon(drawList, center, IM_COL32(120, 130, 145, 100));
		} else if(enabled) {
			ImU32 iconColor = hovered ? IM_COL32(100, 175, 255, 255) : IM_COL32(120, 155, 205, 255);
			DrawBellIcon(drawList, center, iconColor);
		} else {
			DrawBellIcon(drawList, center, IM_COL32(90, 95, 105, 130));
		}
		ImGui::PopID();
		return clicked;
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
		drawList->PathStroke(color, 0, thickness);
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
			drawList->AddRect(pos, ImVec2(pos.x + size, pos.y + size), IM_COL32(70, 130, 220, 100), 9.0f, 0, 1.0f);
		}
		ImGui::PopID();
		return clicked;
	}

	void UI::init() {
		SetupStyle();
		SetupFont();
	}

};