#include "SettingsTab.h"

#include "../../Config/Config.h"
#include "../GUI.h"
#include "../UI.h"
#include "../Tabs.h"

#include <imgui.h>

void SettingsTab::Render() {
	//========================================================
	// Back Button
	//========================================================
	ImGui::SetCursorPos(ImVec2(32.0f, 27.0f));

	if (UI::ModernButton(
		"← Back",
		ImVec2(80.0f, 32.0f),
		ImVec4(0.08f, 0.10f, 0.14f, 1.0f),
		ImVec4(0.12f, 0.14f, 0.19f, 1.0f),
		ImVec4(0.10f, 0.12f, 0.17f, 1.0f)
	)) {
		tab.setTab(Tab::Home);
	}
	//========================================================
	// Header
	//========================================================
	ImGui::SetCursorPos(ImVec2(42.0f, 64.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, UI::Muted);
	ImGui::Text("Configure DevlitchInput");
	ImGui::PopStyleColor();
	//========================================================
	// Settings Card
	//========================================================
	ImGui::SetCursorPos(ImVec2(32.0f, 110.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, UI::Card);
	ImGui::BeginChild("##SettingsCard", ImVec2(ImGui::GetWindowWidth() - 64.0f, ImGui::GetWindowHeight() - 200.0f), true);
	const float cardWidth = ImGui::GetWindowWidth();
	const float cardHeight = ImGui::GetWindowHeight();
	//========================================================
	// Card Header
	//========================================================
	ImGui::SetCursorPos(ImVec2(22.0f, 18.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, UI::Text);
	ImGui::Text("Application");
	ImGui::PopStyleColor();
	ImGui::SetCursorPos(ImVec2(22.0f, 46.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, UI::Muted);
	ImGui::Text("Configure application behavior");
	ImGui::PopStyleColor();
	//========================================================
	// Minimize to tray
	//========================================================
	ImGui::SetCursorPos(ImVec2(22.0f, 90.0f));
	UI::ModernCheckbox("Minimize to tray", &gui.options.minimizeToTray);
	//========================================================
	// Close to tray
	//========================================================
	ImGui::SetCursorPos(ImVec2(22.0f, 132.0f));
	UI::ModernCheckbox("Close to tray", &gui.options.closeToTray);
	//========================================================
	// Session Notice
	//========================================================
	ImGui::SetCursorPos(ImVec2(22.0f, 178.0f));

	ImGui::PushStyleColor(ImGuiCol_Text, UI::Dim);

	ImGui::TextWrapped(
		"Changes are temporary and will be lost when the "
		"application closes unless you save them."
	);

	ImGui::PopStyleColor();

    //========================================================
    // Check Saving State
    //========================================================
    if (saveState == SaveState::Saving) {

        // هل عملية الحفظ خلصت؟
        if (saveFuture.valid() &&
            saveFuture.wait_for(std::chrono::milliseconds(0))
            == std::future_status::ready) {

            bool success = saveFuture.get();

            if (success) {
                saveState = SaveState::Saved;
                savedTime = std::chrono::steady_clock::now();
            }
            else {
                saveState = SaveState::Idle;
            }
        }
    }

    //========================================================
    // Saved Timer
    //========================================================
    if (saveState == SaveState::Saved) {

        auto elapsed =
            std::chrono::steady_clock::now() - savedTime;

        if (elapsed >= std::chrono::milliseconds(500)) {
            saveState = SaveState::Idle;
        }
    }

    //========================================================
    // Save Button
    //========================================================
    ImGui::SetCursorPos(ImVec2(
        cardWidth - 122.0f,
        cardHeight - 54.0f
    ));

    if (saveState == SaveState::Idle) {

        if (UI::ModernButton(
            "Save",
            ImVec2(100.0f, 36.0f),
            ImVec4(0.05f, 0.18f, 0.10f, 1.0f),
            ImVec4(0.07f, 0.32f, 0.17f, 1.0f),
            ImVec4(0.05f, 0.22f, 0.12f, 1.0f)
        )) {

            saveState = SaveState::Saving;

            saveFuture = std::async(
                std::launch::async,
                []() -> bool {
                    return cfg.SaveConfig();
                }
            );
        }
    } else if (saveState == SaveState::Saving) {

        UI::ModernButton(
            "Saving...",
            ImVec2(100.0f, 36.0f),
            ImVec4(0.08f, 0.10f, 0.14f, 1.0f),
            ImVec4(0.08f, 0.10f, 0.14f, 1.0f),
            ImVec4(0.08f, 0.10f, 0.14f, 1.0f)
        );
    } else if (saveState == SaveState::Saved) {

        UI::ModernButton(
            "Saved",
            ImVec2(100.0f, 36.0f),
            ImVec4(0.05f, 0.18f, 0.10f, 1.0f),
            ImVec4(0.05f, 0.18f, 0.10f, 1.0f),
            ImVec4(0.05f, 0.18f, 0.10f, 1.0f)
        );
    }

	ImGui::EndChild();
	ImGui::PopStyleColor();
}