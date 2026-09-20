#include "TCPConfig.h"

#include "Shared/Protocol.h"

#include <algorithm>
#include <cstring>
#include <string_view>

class TcpServer {
public:
	bool init();
	TcpConfig func{0};
	bool IsRunning() const {
		return func.IsRunning();
	}
	void RefreshControllerList();
private:
	template <std::size_t N>
	void setString(char(&dest)[N], std::string_view value){
		const auto size = std::min(value.size(), N - 1);
		std::memcpy(dest, value.data(), size);
		dest[size] = '\0';
	}
private:
	void HandleControllerList();
	void HandleControllerState(uint32_t controllerId, bool type);
	void HandleControllerRumble(IPC::TestRumblePayload payload);
};

inline TcpServer server;