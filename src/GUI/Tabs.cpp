#include "Tabs.h"

#include "GUI.h"
#include "UI.h"
#include "../Steam/Login/Selector.h"
#include "../Steam/VDF/checker.h"
#include "../Config/Config.h"
#include "../Utils/Utils.h"

void TabManager::setTab(Tab tab) {
    currentTab_ = tab;
}

Tab TabManager::currentTab() const {
    return currentTab_;
}

void TabManager::RenderFooter() {
    std::string leftTextO = "SteamUser: " + cfg.User;
    const char* leftTextT = "https://github.com/devlitch/DevlitchInput";
    const char* versionText = "v1.6";

    const float footerY = ImGui::GetWindowHeight() - 35.0f;
    const float rightPadding = 7.0f;
    const float gap = 10.0f;

    const ImVec2 switchSize(105.0f, 32.0f);

    const float versionWidth = ImGui::CalcTextSize(versionText).x;

    //========================================================
    // Left
    //========================================================
    ImGui::SetCursorPos(ImVec2(7.0f, footerY - 17.0f));

    ImGui::PushStyleColor(ImGuiCol_Text, UI::Text);
    ImGui::TextUnformatted(leftTextO.c_str());
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(7.0f, footerY + 3.0f));

    ImGui::PushStyleColor(ImGuiCol_Text, UI::Dim);
    ImGui::TextUnformatted(leftTextT);
    ImGui::PopStyleColor();

    //========================================================
    // Version
    //========================================================
    const float versionX =
        ImGui::GetWindowWidth()
        - versionWidth
        - rightPadding;

    ImGui::SetCursorPos(ImVec2(versionX, footerY));

    ImGui::PushStyleColor(ImGuiCol_Text, UI::Dim);
    ImGui::TextUnformatted(versionText);
    ImGui::PopStyleColor();

    //========================================================
    // Switch User
    //========================================================
    const float switchX =
        versionX
        - switchSize.x
        - gap;

    ImGui::SetCursorPos(ImVec2(switchX, footerY - 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.85f));
    if (UI::ModernButton(
        "Switch user",
        switchSize,
        ImVec4(0.07f, 0.09f, 0.13f, 1.0f),
        ImVec4(0.11f, 0.14f, 0.20f, 1.0f),
        ImVec4(0.09f, 0.12f, 0.17f, 1.0f),
        true
    )) {
        std::thread([&] {
            gui.Pause();
            gui.Resume();
            std::string oldUser = cfg.User;
            UserSelector{}.init();
            if (oldUser != cfg.User) {
                gui.LoadingText("Exiting...");
                RestartApp();
            } else {
                gui.SetRender([&] {
                    Render();
                });
                gui.LoadingOff();
            }
        }).detach();
    }
    ImGui::PopStyleVar();
}

void TabManager::init() {
    gui.SetRender([&]() {
        Render();
    });
    gui.LoadingOff();
    gui.Wait();
}

void TabManager::Render() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::Begin("Controller Manager", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    switch (currentTab_) {
    case Tab::Home:
        ControllerHub.Render();
        break;

    case Tab::Settings:
        SettingsTab.Render();
        break;

    default:
        break;
    }

    RenderFooter();

    ImGui::End();
}