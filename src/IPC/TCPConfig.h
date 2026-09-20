#pragma once
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <asio.hpp>
#include <SDL3/SDL_messagebox.h>

#include <cstdint>
#include <mutex>
#include <string>
#include <type_traits>
#include <filesystem>

#include "Shared/Protocol.h"


class TcpConfig {
public:
    explicit TcpConfig(uint16_t port);
    ~TcpConfig();
public:
    bool Start();
    void Stop();
    bool IsRunning() const;
    bool IsClientConnected() const;
    uint16_t GetPort() const;
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
    std::filesystem::path GetTempDir();
    bool CreateAcceptor();
    bool WaitForClient();
    bool checkMsg(std::string msg);
private:
    asio::io_context m_ioContext;
    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::socket m_socket;

    uint16_t m_port;

    bool m_running = false;
    std::mutex m_writeMutex;
    std::filesystem::path portPath;
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
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput - SP",
            (
                std::string("[SP] Invalid payload size. Expected=") + std::to_string(sizeof(TPayload)) +
                " Received=" + std::to_string(payloadSize)
            ).c_str(),
            nullptr
        );
        return false;
    }
    return ReceivePayload(&payload, payloadSize);
}