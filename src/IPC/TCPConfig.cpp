#include "TcpConfig.h"

#include <fstream>
#include <format>
#include <vector>

TcpConfig::TcpConfig(uint16_t port) : m_acceptor(m_ioContext), m_socket(m_ioContext), m_port(port) {}

TcpConfig::~TcpConfig() {
    Stop();
}

//==================================================
// Server Lifecycle
//==================================================

bool TcpConfig::Start() {
    if (m_running) return true;

    try {
        if (!CreateAcceptor()) return false;
        bool IsConnected = WaitForClient();
        if (std::filesystem::exists(portPath)) std::filesystem::remove(portPath);
        if (!IsConnected) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput", "Please launch it from \"DevlitchInput.exe\".", nullptr);
            Stop();
            return false;
        }
        m_running = true;
        return true;
    } catch (const std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput - SP", e.what(), nullptr);
        Stop();
        return false;
    }
}

void TcpConfig::Stop() {
    m_running = false;
    std::error_code ec;

    if (m_socket.is_open()) {
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close(ec);
    }


    if (m_acceptor.is_open()) m_acceptor.close(ec);
}


bool TcpConfig::IsRunning() const {
    return m_running;
}


bool TcpConfig::IsClientConnected() const {
    return m_socket.is_open();
}


uint16_t TcpConfig::GetPort() const
{
    return m_port;
}


//==================================================
// Acceptor
//==================================================

std::filesystem::path TcpConfig::GetTempDir() {
    return std::filesystem::temp_directory_path();
}

bool TcpConfig::CreateAcceptor() {
    try {
        const asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), m_port);

        m_acceptor.open(endpoint.protocol());
        m_acceptor.set_option(asio::socket_base::reuse_address(true));
        m_acceptor.bind(endpoint);
        m_acceptor.listen(asio::socket_base::max_listen_connections);

        m_port = m_acceptor.local_endpoint().port();

        portPath = GetTempDir() / "DevlitchInput_Port";
        std::ofstream out(portPath);
        out << GetPort();

        return true;
    } catch (const std::exception& e) {
        std::string msg = e.what();
        if (checkMsg(msg)) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput", std::format("[SP] CreateAcceptor failed: {}", msg).c_str(), nullptr);
        return false;
    }
}


//==================================================
// Accept Client
//==================================================

bool TcpConfig::WaitForClient() {
    try {
        // Waiting for client...
        m_acceptor.accept(m_socket);
        // Client connected
        return true;
    } catch (const std::exception& e) {
        std::string msg = e.what();
        if (checkMsg(msg)) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput", std::format("[SP] Accept failed: {}", msg).c_str(), nullptr);
        return false;
    }
}


//==================================================
// Send
//==================================================

bool TcpConfig::Send(IPC::PacketType type) {
    return SendRaw(type, nullptr, 0);
}


bool TcpConfig::SendRaw(IPC::PacketType type, const void* data, uint32_t size) {
    if (!IsClientConnected()) return false;

    std::lock_guard<std::mutex> lock(m_writeMutex);

    try {
        IPC::PacketHeader header{};

        header.type = type;
        header.payloadSize = size;
        asio::write(m_socket, asio::buffer(&header, sizeof(header)));
        if (size > 0) {
            if (data == nullptr) return false;
            asio::write(m_socket, asio::buffer(data,size));
        }
        return true;
    } catch (const std::exception& e) {
        std::string msg = e.what();
        if (checkMsg(msg)) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput", std::format("[SP] Send failed: {}", msg).c_str(), nullptr);
        return false;
    }
}


//==================================================
// Receive
//==================================================

bool TcpConfig::ReceiveHeader(IPC::PacketHeader& header) {
    if (!IsClientConnected()) return false;

    try {
        asio::read(m_socket, asio::buffer(&header, sizeof(header)));
        return true;
    } catch (const std::exception& e) {
        std::string msg = e.what();
        if (checkMsg(msg)) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput", std::format("[SP] ReceiveHeader failed: {}", msg).c_str(), nullptr);
        return false;
    }
}


bool TcpConfig::ReceivePayload(void* buffer, uint32_t size) {
    if (!IsClientConnected()) return false;
    if (size == 0) return true;
    if (buffer == nullptr) return false;

    try {
        asio::read(m_socket, asio::buffer(buffer, size));
        return true;
    } catch (const std::exception& e) {
        std::string msg = e.what();
        if (checkMsg(msg)) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DevlitchInput", std::format("[SP] ReceivePayload failed: {}", msg).c_str(), nullptr);
        return false;
    }
}

bool TcpConfig::checkMsg(std::string msg) {
    static const std::vector<std::string> blocked = {
        "A blocking operation was interrupted by a call to WSACancelBlockingCall",
        "An existing connection was forcibly closed by the remote host",
        "End of file"
    };

    if (!msg.empty() && msg.back() == '.') msg.pop_back();

    for (const auto& suffix : blocked) {
        if (msg.ends_with(suffix)) return false;
    }
    return true;
}