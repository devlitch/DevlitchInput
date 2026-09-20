#pragma once
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <asio.hpp>

#include <cstdint>
#include <mutex>
#include <string>
#include <type_traits>
#include <vector>
#include <atomic>

#include "../GUI/GUI.h"
#include "Shared/Protocol.h"


class TcpConfig {
public:
    TcpConfig();
    void SetEndpoint(const std::string& host, uint16_t port);
    ~TcpConfig();
public:
    bool Connect();
    void Disconnect();
    bool IsConnected() const;
public:
    bool Send(IPC::PacketType type);
    template<typename TPayload>
    bool Send(IPC::PacketType type, const TPayload& payload);
    bool SendRaw(IPC::PacketType type, const void* data, uint32_t size);
public:
    bool ReceiveHeader(IPC::PacketHeader& header);
    bool ReceivePayload(void* buffer, uint32_t size);
    template<typename TPayload>
    bool ReceivePayload(TPayload& payload, uint32_t payloadSize);
private:
    bool ReadExact(void* buffer, std::size_t size);
    bool WriteExact(const void* buffer, std::size_t size);
private:
    std::thread m_ioThread;
    std::string m_host;
    uint16_t m_port;

    std::atomic_bool m_stopping{ false };

    asio::io_context m_ioContext;
    asio::ip::tcp::socket m_socket;

    mutable std::mutex m_writeMutex;
};


//==================================================
// Template Implementations
//==================================================

template<typename TPayload>
bool TcpConfig::Send(IPC::PacketType type, const TPayload& payload) {
    static_assert(std::is_trivially_copyable_v<TPayload>, "Payload must be trivially copyable");

    return SendRaw(type, &payload, static_cast<uint32_t>(sizeof(TPayload)));
}

template<typename TPayload>
bool TcpConfig::ReceivePayload(TPayload& payload, uint32_t payloadSize) {
    static_assert(std::is_trivially_copyable_v<TPayload>, "Payload must be trivially copyable");
    if (payloadSize != sizeof(TPayload)) {
        gui.ShowError(
            std::string("[Client] Invalid payload size.\n") +
            "Expected= " + std::to_string(sizeof(TPayload)) +
            " Received=" + std::to_string(payloadSize)
        );
        return false;
    }
    return ReceivePayload(&payload, payloadSize);
}