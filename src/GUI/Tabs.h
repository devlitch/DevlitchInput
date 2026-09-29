#pragma once

#include "Tabs/ControllerHub.h"
#include "Tabs/SettingsTab.h"

enum class Tab {
    Home,
    Settings
};

class TabManager {
private:
    Tab currentTab_ = Tab::Home;
    ControllerHub ControllerHub;
    SettingsTab SettingsTab;

    void RenderFooter();

public:
    void setTab(Tab tab);
    Tab currentTab() const;
    void init();
    void Render();
};

inline TabManager tab;