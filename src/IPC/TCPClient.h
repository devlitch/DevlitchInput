#include "TCPConfig.h"

#include <string>
#include <cstdint>

class TcpClient {
public:
	bool init();
	TcpConfig func;
private:
	int GetCurrentPort();
	bool IsValidPort(const std::string& value, uint16_t& port);
};

inline TcpClient client;