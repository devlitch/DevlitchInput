#include "TCPServer.h"

#include "../Input/ControllerManager.h"

#include <cstddef>
#include <cstdint>

bool TcpServer::init() {
    if (!server.func.Start()) return 0;

    while (server.func.IsRunning()) {
        IPC::PacketHeader header{};

        if (!server.func.ReceiveHeader(header)) {
            server.func.Stop();
            break;
        }
        switch (header.type) {
        case IPC::PacketType::ListControllers: {
            HandleControllerList();
            break;
        }
        case IPC::PacketType::Connect:
        case IPC::PacketType::Disconnect: {
            IPC::ControllerPayload payload{};
            if (!server.func.ReceivePayload(payload, header.payloadSize)) break;
            HandleControllerState(payload.controllerId, header.type == IPC::PacketType::Connect ? true : false);
            break;
        }
        case IPC::PacketType::TestRumble: {
            IPC::TestRumblePayload payload{};
            if (!server.func.ReceivePayload(payload, header.payloadSize)) break;
            HandleControllerRumble(payload);
            break;
        }
        default: 
            break;
        }
    }
    return 1;
}

void TcpServer::RefreshControllerList() {
    HandleControllerList();
}

void TcpServer::HandleControllerList() {
    controllerManager.Refresh();
    const auto& controllerList = controllerManager.GetControllers();
    IPC::ControllerListPayload response{};
    const size_t count = (std::min)(controllerList.size(), static_cast<size_t>(IPC::MAX_CONTROLLERS));
    response.count = static_cast<uint32_t>(count);
    for (size_t i = 0; i < count; i++) {
        const auto& controller = controllerList[i];
        response.controllers[i].id = static_cast<int64_t>(controller.id);
        response.controllers[i].connected = controller.connected ? 1 : 0;
        setString(response.controllers[i].name, controller.name);
    }
    server.func.Send(IPC::PacketType::ControllerList, response);
}

void TcpServer::HandleControllerState(uint32_t controllerId, bool type) {
    if (controllerManager.setControllerState(controllerId, type)) {
        IPC::ControllerPayload response{};
        response.controllerId = controllerId;
        server.func.Send(type ? IPC::PacketType::ControllerConnected : IPC::PacketType::ControllerDisconnected, response);
    } else {
        IPC::ErrorPayload response{};
        response.type = IPC::ErrorType::Connect;
        response.errorCode = (type ? 100 : 0) + controllerId;
        server.func.Send(IPC::PacketType::Error, response);
    }
}

void TcpServer::HandleControllerRumble(IPC::TestRumblePayload payload) {
    if (controllerManager.Rumble(payload.controllerId, payload.smallMotor, payload.largeMotor, payload.durationMs)) {
        IPC::ControllerPayload response{};
        response.controllerId = payload.controllerId;
        server.func.Send(IPC::PacketType::ControllerPinged, response);
    } else {
        IPC::ErrorPayload response{};
        response.type = IPC::ErrorType::Ping;
        response.errorCode = payload.controllerId;
        server.func.Send(IPC::PacketType::Error, response);
    }
}