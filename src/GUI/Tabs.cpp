#include "Tabs.h"

void TabManager::setTab(Tab tab) {
    currentTab_ = tab;
}

Tab TabManager::currentTab() const {
    return currentTab_;
}

void TabManager::Render() {
    switch (currentTab_) {
    case Tab::Home:
        ControllerHub.init();
        break;

    default:
        break;
    }
}