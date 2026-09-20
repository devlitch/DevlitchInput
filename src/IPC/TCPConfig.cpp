#include "TcpConfig.h"

#include <stdexcept>

TcpConfig::TcpConfig() : m_socket(m_ioContext) {}

void TcpConfig::SetEndpoint(const std::string& host, uint16_t port) {
    m_host = host;
    m_port = port;
}

TcpConfig::~TcpConfig() {
    Disconnect();
}


//==================================================
// Connection
//==================================================

bool TcpConfig::Connect() {
    if (IsConnected()) return true;

    try {
        asio::ip::tcp::resolver resolver(m_ioContext);

        const auto endpoints = resolver.resolve(m_host, std::to_string(m_port));
        asio::error_code ec;
        asio::connect(m_socket, endpoints, ec);

        if (ec) {
            m_socket.close();
            gui.ShowError(std::string("[IPC] Connect failed: \n") + ec.message());
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        m_socket.close();
        gui.ShowError(std::string("[IPC] Connect failed[ex]: \n") + e.what());
        return false;
    }
}

void TcpConfig::Disconnect() {
    if (!m_socket.is_open()) return;
    std::error_code ec;
    m_stopping = 1;
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
    gui.Stop();
}


bool TcpConfig::IsConnected() const {
    return m_socket.is_open();
}


//==================================================
// Send
//==================================================

bool TcpConfig::Send(IPC::PacketType type){
    return SendRaw(type, nullptr, 0);
}


bool TcpConfig::SendRaw(IPC::PacketType type, const void* data, uint32_t size){
    if (!IsConnected()) return false;
    std::lock_guard<std::mutex> lock(m_writeMutex);

    try {
        IPC::PacketHeader header{};

        header.type = type;
        header.payloadSize = size;

        asio::write(m_socket, asio::buffer(&header, sizeof(header)));
        if (size > 0) {
            if (data == nullptr) return false;
            asio::write(m_socket, asio::buffer(data, size));
        }
        return true;
    }
    catch (const std::exception& e) {
        gui.ShowError(std::string("[IPC] Send failed: \n") + e.what());
        return false;
    }
}


//==================================================
// Receive
//==================================================

bool TcpConfig::ReceiveHeader(IPC::PacketHeader& header) {
    if (!IsConnected()) return false;

    try {
        asio::read(m_socket, asio::buffer(&header, sizeof(header)));
        return true;
    } catch (const std::exception& e) {
        if (m_stopping) return false;
        gui.ShowError(std::string("[IPC] ReceiveHeader failed: \n") + e.what());
        return false;
    }
}

bool TcpConfig::ReceivePayload(void* buffer, uint32_t size) {
    if (!IsConnected()) return false;
    if (size == 0) return true;
    if (buffer == nullptr) return false;

    try {
        asio::read(m_socket, asio::buffer(buffer, size));
        return true;
    } catch (const std::exception& e) {
        gui.ShowError(std::string("[IPC] ReceivePayload failed: \n") + e.what());
        return false;
    }
}

//==================================================
// Internal
//==================================================

bool TcpConfig::ReadExact(void* buffer, std::size_t size){
    if (!IsConnected()) return false;
    if (size == 0) return true;
    try {
        asio::read(m_socket, asio::buffer(buffer, size));
        return true;
    } catch (...) {
        return false;
    }
}


bool TcpConfig::WriteExact(const void* buffer, std::size_t size) {
    if (!IsConnected()) return false;
    if (size == 0) return true;
    try {
        asio::write(m_socket, asio::buffer(buffer, size));
        return true;
    } catch (...) {
        return false;
    }
}