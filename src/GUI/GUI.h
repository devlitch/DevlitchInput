#pragma once

#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include <type_traits>
#include <memory>
#include <utility>


#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "Renderer.h"

class GUI {
public:
    bool Init();

    void ProcessEvent(SDL_Event* event);

    using RenderCallback = std::function<void()>;
    void SetRender(RenderCallback callback);
public:
    bool isLoading() const;
    void LoadingOn();
    void LoadingText(const char* text);
    void LoadingOff();
public:
    template<typename F>
    auto RunOnThread(F&& func);

    bool isActive() const;
    bool isRunning() const;
    void Resume();
    void Pause();
    void Stop();
    void Wait(bool t = false);

    void Dragable(bool t = 1);
    void ShowWindow(bool t = 1);
    void Borderless(bool t = 1);
    void SetSize(int width = 1280, int height = 720);
    void RaiseWindow();
    void BringWindowToFront();

    void ShowError(std::string text);

    Renderer renderer;
    SDL_Window* window = nullptr;
    SDL_GPUDevice* device = nullptr;
    std::thread guiThread;
private:
    std::atomic<bool> running{true};
    std::atomic<bool> alive{true};
    
    std::thread::id guiThreadId;
    void Run();
    template<typename F>
    void RunOnThreadSafe(F&& func);
    void Shutdown();
private:
    std::mutex taskMutex;
    std::queue<std::function<void()>> tasks;
    std::atomic<bool> taskPending{ false };
private:
    static SDL_HitTestResult SDLCALL HitTest(SDL_Window* window, const SDL_Point* pt, void* data);
    bool AttachWindow(SDL_Window* window);

    void BeginFrame();
    void Render();
    RenderCallback renderCallback;
};

template<typename F>
auto GUI::RunOnThread(F&& func) {
    using R = std::invoke_result_t<F>;

    auto task = std::make_shared<std::packaged_task<R()>>(
        std::forward<F>(func)
    );

    auto future = task->get_future();

    if (std::this_thread::get_id() == guiThreadId) {
        (*task)();
        return future;
    }

    {
        std::lock_guard lock(taskMutex);
        tasks.push([task]() {
            (*task)();
        });
    }

    taskPending.store(true);

    return future;
}

template<typename F>
void GUI::RunOnThreadSafe(F&& func) {
    auto future = RunOnThread(std::forward<F>(func));

    if (alive.load()) {
        future.get();
    }
}

inline GUI gui;