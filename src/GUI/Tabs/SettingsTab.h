#pragma once

#include <chrono>
#include <future>

class SettingsTab {
public:
    void Render();
private:
    enum class SaveState {
        Idle,
        Saving,
        Saved
    };

    SaveState saveState = SaveState::Idle;
    std::future<bool> saveFuture;
    std::chrono::steady_clock::time_point savedTime;
};