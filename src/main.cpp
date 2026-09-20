#include <thread>

#include "IPC/TCPServer.h"
#include "Gamepad/GamepadManager.h"
#include "Bridge/ViGEmManager.h"

std::thread serverThread;

void Stop() {
    bridge.Shutdown();
    server.func.Stop();
    if (serverThread.joinable()) serverThread.join();
    SDL_Quit();
}
int main() {
    serverThread = std::thread(&TcpServer::init, &server);

    if (!bridge.Initialize()) {
        IPC::ErrorPayload response{};

        response.type = IPC::ErrorType::ViGEmInit;
        server.func.Send(IPC::PacketType::Error, response);
        Stop();
        return -1;
    }

    if (!gamepadManager.Initialize()) {
        IPC::ErrorPayload response{};

        response.type = IPC::ErrorType::SDLInit;

        server.func.Send(IPC::PacketType::Error, response);
        Stop();
        return -1;
    }

    {
        IPC::SuccessPayload response{};
        response.type = IPC::SuccessType::StartUI;
        server.func.Send(IPC::PacketType::Success, response);
    }

    gamepadManager.Start();

    Stop();

    return 0;
}