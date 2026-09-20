#pragma once

#include <imgui.h>

namespace UI {
	extern const ImVec4 Bg;
	extern const ImVec4 Card;
	extern const ImVec4 CardHover;

	extern const ImVec4 Text;
	extern const ImVec4 Muted;
	extern const ImVec4 Dim;

	extern const ImVec4 Blue;
	extern const ImVec4 BlueHover;
	extern const ImVec4 Cyan;

	extern const ImVec4 Green;
	extern const ImVec4 Red;
	extern const ImVec4 Yellow;

	extern const ImVec4 Border;
	extern const ImVec4 BorderHover;

	bool Button(const char* label, bool active, ImVec2 size);
	bool ModernButton(const char* label, ImVec2 size, ImVec4 normal, ImVec4 hover, ImVec4 active);
	void DrawLED(ImDrawList* drawList, ImVec2 center, bool connected);
	bool BellButton(const char* id, bool enabled, bool pending);
	void DrawRefreshIcon(ImDrawList* drawList, ImVec2 center, ImU32 color, float radius = 7.0f, float rotation = 0.0f);
	bool RefreshButton(const char* id, bool pending);

	class UI {
	public:
		void init();
	private:
		void SetupStyle();
		void SetupFont();
	};
}
inline UI::UI ui;