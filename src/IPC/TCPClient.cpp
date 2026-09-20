#include "TCPClient.h"

#include <SDL3/SDL.h>
#include <thread>
#include <chrono>

#include <fstream>
#include <charconv>
#include <cctype>

#include "../Utils/Utils.h"
#include "../GUI/GUI.h"

#include "Shared/Protocol.h"

bool TcpClient::init() {
    gui.LoadingText("Waiting DevlitchInput_SP");
    int port = GetCurrentPort();
    if (port <= 0) return false;
    func.SetEndpoint("127.0.0.1", port);
    if (!func.Connect()) {
        std::string target = GetProgramFolder() + "\\sp.exe";
        bool r = IsExeRunning(std::wstring(target.begin(), target.end()));
        if (!r) gui.ShowError("There's an issue while trying to connect to the Steam part.");
        return false;
    }
    return true;
}

int TcpClient::GetCurrentPort() {
    while (!std::filesystem::exists(portPath)) {
        if (!gui.isActive()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (!std::filesystem::exists(portPath)) return 0;
    std::ifstream in(portPath);
    if (!in) return -1;
    std::string line;
    if (!std::getline(in, line)) return -1;
    uint16_t port{};
    if (!IsValidPort(line, port)) return -1;
    return static_cast<int>(port);
    return true;
}

bool TcpClient::IsValidPort(const std::string& value, uint16_t& port) {
    if (value.empty()) return false;
    for (unsigned char c : value) {
        if (!std::isdigit(c)) return false;
    }
    unsigned int number = 0;
    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), number);
    if (ec != std::errc{} || ptr != value.data() + value.size()) return false;
    if (number < 1 || number > 65535) return false;
    port = static_cast<uint16_t>(number);
    return true;
}