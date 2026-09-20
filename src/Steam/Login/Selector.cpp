#include "Selector.h"

#include "imgui.h"

#include <cstdint>

#include "assets/guest.h"
#include "../../GUI/GUI.h"
#include "LoginVDF.h"
#include "../Config/Config.h"

void UserSelector::init() {
    gui.LoadingText("UserSelector$ Loading");
    LoginVDF loginVDF;
    users = loginVDF.LoadSteamUsers(cfg.SteamFolder);
    gui.ShowWindow(0);
    gui.SetSize(800, 600);
    gui.Borderless();
    gui.Dragable();
    gui.ShowWindow();

    gui.RunOnThread([&] {
        LoadAvatars(gui.renderer);
     }).get();

    gui.SetRender([&](){
        Render();
    });

    gui.LoadingOff();
    gui.Wait();

    gui.Resume();
    gui.SetSize();
    gui.Borderless(0);
    gui.Dragable(0);
    gui.LoadingOn();
    gui.ShowWindow();
}

Texture* UserSelector::LoadAvatar(Renderer& renderer, std::string& file) {
    Texture* avatar = renderer.LoadTexture(file.c_str());
    return avatar;
}

void UserSelector::LoadAvatars(Renderer& renderer) {
	for (auto& user : users) {
        user.avatar = user.avatarUrl.empty()?renderer.LoadTextureFromMemory(guest_devlitch_img,guest_devlitch_img_size):LoadAvatar(renderer, user.avatarUrl);
	}
}

void UserSelector::Render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("Users", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 35.0f, 5.0f));
    if (ImGui::Button("X", ImVec2(25, 25))) gui.Stop();
    const int userCount = static_cast<int>(users.size());
    if (userCount > 0) {
        const int itemsPerRow = 7;
        float itemWidth = 70.0f;
        float spacing = ImGui::GetStyle().ItemSpacing.x;

        float rowWidth = userCount < itemsPerRow ?
            userCount * itemWidth + (userCount - 1) * spacing :
            itemsPerRow * itemWidth + (itemsPerRow - 1) * spacing;

        float itemHeight = 70.0f + ImGui::GetTextLineHeightWithSpacing();
        float startY = (ImGui::GetWindowHeight() - itemHeight) * 0.5f;
        ImGui::SetCursorPosY(startY);

        for (int i = 0; i < userCount; i++) {
            if (i % itemsPerRow == 0) {
                int remaining = userCount - i;
                int itemsInRow = std::min(itemsPerRow, remaining);
                float rowWidth = itemsInRow * itemWidth + (itemsInRow - 1) * spacing;
                float startX = (ImGui::GetWindowWidth() - rowWidth) * 0.5f;
                ImGui::SetCursorPosX(startX);
            }
            ImGui::BeginGroup();

            bool hovered = false;

            ImGui::Image((ImTextureID)users[i].avatar->Get(),ImVec2(70, 70));
            if (ImGui::IsItemClicked()) {
                gui.ShowWindow(0);
                cfg.User = users[i].name.c_str();
                uint64_t accountId = (uint64_t)(users[i].id - 76561197960265728ULL);
                cfg.accountId = std::to_string(accountId);
                cfg.SaveConfig();
                gui.Pause();
            }
            hovered = ImGui::IsItemHovered();
            if (hovered) {
                ImDrawList* draw = ImGui::GetWindowDrawList();
                draw->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

                const char* name = users[i].name.c_str();
                ImVec2 textSize = ImGui::CalcTextSize(name);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (70.0f - textSize.x) * 0.5f);
                ImGui::TextUnformatted(name);
            } else {
                ImGui::Dummy(ImVec2(70.0f, ImGui::GetTextLineHeightWithSpacing()));
            }

            ImGui::EndGroup();

            if ((i + 1) % itemsPerRow != 0) ImGui::SameLine();
        }
    }
    ImGui::End();
}