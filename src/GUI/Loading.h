#pragma once

namespace Loading {
	void On();
	void Off();
	bool IsActive();
	void Text(const char* reason);
	void Update(float deltaTime);
	void Draw();
}