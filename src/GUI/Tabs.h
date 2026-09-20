#pragma once

#include "Tabs/ControllerHub.h"

enum class Tab {
    Home
};

class TabManager {
private:
    Tab currentTab_ = Tab::Home;
    ControllerHub ControllerHub;

public:
    void setTab(Tab tab);
    Tab currentTab() const;
    void Render();
};

inline TabManager tab;