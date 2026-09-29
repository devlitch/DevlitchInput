#pragma once

#include <windows.h>

class SingleInstance {
public:
    SingleInstance() = default;
    ~SingleInstance();

    bool Acquire();
    void Release();

    SingleInstance(const SingleInstance&) = delete;
    SingleInstance& operator=(const SingleInstance&) = delete;

private:
    void* mutex_ = nullptr;
};
