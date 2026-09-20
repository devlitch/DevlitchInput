#pragma once

#include <cstdint>

namespace IPC {
    //==================================================
    // Packet Types
    //==================================================

    enum class PacketType : uint16_t {

        ListControllers = 1,
        Connect,
        Disconnect,
        TestRumble,

        Success = 100,
        Error,
        ControllerList,

        ControllerConnected = 200,
        ControllerDisconnected,
        ControllerPinged,
        InputState
    };

    enum class RequestState {
        Idle,
        Pending,
        Success,
        Failed
    };

    enum class SuccessType : uint8_t {
        StartUI = 1,
        Ping,

        Unknown = 255
    };

    enum class ErrorType : uint8_t {
        ViGEmInit = 1,
        SDLInit,
        Ping,
        Connect,

        Unknown = 255
    };

    //==================================================
    // Common Packet Header
    //==================================================

#pragma pack(push, 1)

    struct PacketHeader {
        PacketType type;
        uint32_t payloadSize;
    };

    //==================================================
    // Request Payloads
    //==================================================

    struct ControllerPayload {
        uint32_t controllerId;
    };

    struct TestRumblePayload {
        uint32_t controllerId;

        uint16_t largeMotor;
        uint16_t smallMotor;

        uint16_t durationMs;
    };

    //==================================================
    // Response Payloads
    //==================================================

    struct SuccessPayload {
        //uint8_t success;
        SuccessType type;
    };

    struct ErrorPayload {
        uint8_t errorCode;
        ErrorType type;
    };

    struct ControllerInfo {
        uint32_t id;
        uint8_t connected;
        char name[128];
    };

    constexpr uint32_t MAX_CONTROLLERS = 16;

    struct ControllerListPayload {
        uint32_t count;
        ControllerInfo controllers[MAX_CONTROLLERS];
    };

#pragma pack(pop)

}