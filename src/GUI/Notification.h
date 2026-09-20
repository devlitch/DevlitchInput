#pragma once

#include <imgui.h>

#include <mutex>
#include <string>

namespace Notification {
	enum class Type {
		None, Success, Error, Warning, Info
	};
	struct State {
		Type type = Type::None;
		std::string message;
		float duration = 0.0f;
		float timeLeft = 0.0f;
	};
}
class NotificationManager {
public: void Show(Notification::Type type, const std::string& message, float duration = 3.0f);
	  void Render();
	  void Clear();
private: std::mutex mutex;
	   Notification::State notification;
};

inline NotificationManager notificationManager;