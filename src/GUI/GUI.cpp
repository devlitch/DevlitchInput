#include "GUI.h"

#include <csignal>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"

#include "UI.h"
#include "Renderer.h"
#include "Loading.h"
#include "Notification.h"

#include "../DI/nput.h"

bool GUI::Init() {
    guiThread = std::thread(&GUI::Run, this);
    return true;
}

bool GUI::AttachWindow(SDL_Window* currentWindow) {
    if (!device) {
        ShowError("GPU device is null");
        return false;
    }
    if (!SDL_ClaimWindowForGPUDevice(device, currentWindow)) {
        ShowError(std::string("Failed claiming window: \n") + SDL_GetError());
        return false;
    }

    if (!ImGui_ImplSDL3_InitForSDLGPU(currentWindow)) {
        ShowError("ImGui SDL3 init failed");
        return false;
    }

    ImGui_ImplSDLGPU3_InitInfo info{};
    info.Device = device;
    info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(device, currentWindow);
    info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;

    window = currentWindow;

    return ImGui_ImplSDLGPU3_Init(&info);
}

void GUI::ProcessEvent(SDL_Event* event) {
    ImGui_ImplSDL3_ProcessEvent(event);
}

void GUI::BeginFrame() {
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
}

void GUI::Render() {
    ImGui::Render();

    ImDrawData* draw_data = ImGui::GetDrawData();

    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    if (!commandBuffer) return;

    SDL_GPUTexture* swapchainTexture = nullptr;
    bool acquired = SDL_AcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, nullptr, nullptr);
    if (!acquired) {
        SDL_Log("Acquire failed: %s", SDL_GetError());
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return;
    }
    if (!swapchainTexture) {
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return;
    }

    ImGui_ImplSDLGPU3_PrepareDrawData( draw_data, commandBuffer);

    SDL_GPUColorTargetInfo targetInfo{};
    targetInfo.texture = swapchainTexture;
    targetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    targetInfo.store_op = SDL_GPU_STOREOP_STORE;
    targetInfo.clear_color = { 0.1f, 0.1f, 0.1f, 1.0f };

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(commandBuffer, &targetInfo, 1, nullptr);

    if (!pass) {
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return;
    }

    ImGui_ImplSDLGPU3_RenderDrawData(draw_data, commandBuffer, pass);
    SDL_EndGPURenderPass(pass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
}

void GUI::SetRender(RenderCallback callback) {
    renderCallback = std::move(callback);
}

void GUI::Run() {
    guiThreadId = std::this_thread::get_id();
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        ShowError(std::string("SDL Video init failed: \n") + SDL_GetError());
        return;
    }
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
    SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");

    device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV |
        SDL_GPU_SHADERFORMAT_DXIL |
        SDL_GPU_SHADERFORMAT_METALLIB,
        false,
        nullptr
    );


    if (!device) {
        ShowError(std::string("GPU device creation failed: \n") + SDL_GetError());
        return;
    }

    //
    // ImGui
    //

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    if (!renderer.Init(device)) {
        ShowError("Renderer init failed");
        return;
    }
    SDL_Window* currentWindow = SDL_CreateWindow("DevlitchInput", 1200, 720, 0);
    AttachWindow(currentWindow);
    ui.init();
    while (alive.load()) {
        Wait(true);
        if (!alive.load()) break;
        if (taskPending.exchange(false)) {
            std::queue<std::function<void()>> pending;

            {
                std::lock_guard lock(taskMutex);
                std::swap(pending, tasks);
            }

            while (!pending.empty()) {
                pending.front()();
                pending.pop();
            }
        }
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                Stop();
                break;
            }
            if (!isRunning()) continue;
            if (!alive.load()) break;
            ProcessEvent(&e);
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_F5 && !e.key.repeat) {
                di.UpdateControllers();
                SDL_Delay(50);
            }
        }
        if (!alive.load()) break;
        if (!isRunning()) continue;
        BeginFrame();

        if (Loading::IsActive()) {
            ImGuiIO& io = ImGui::GetIO();
            Loading::Update(io.DeltaTime);
            Loading::Draw();
        } else {
            if (renderCallback) renderCallback();
        }

        notificationManager.Render();

        Render();
        SDL_Delay(5);
    }
    Shutdown();
}

bool GUI::isLoading() const {
    return Loading::IsActive();
}

void GUI::LoadingOn() {
    Loading::On();
}

void GUI::LoadingText(const char* text) {
    Loading::Text(text);
}

void GUI::LoadingOff() {
    Loading::Off();
}

bool GUI::isActive() const {
    return alive.load();
}

bool GUI::isRunning() const {
    return running.load();
}

void GUI::Resume() {
    running.store(true);
    running.notify_all();
}

void GUI::Pause() {
    running.store(false);
    running.notify_all();
}

void GUI::Wait(bool t) {
    running.wait(!t);
}

void GUI::Stop() {
    alive.store(false);
    running.store(false);
    running.notify_all();
    alive.notify_all();
}

SDL_HitTestResult SDLCALL GUI::HitTest(SDL_Window* window, const SDL_Point* pt, void* data) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    int buttonSize = 25;
    int padding = 5;

    if (pt->x > w - buttonSize - padding &&
        pt->x < w - padding &&
        pt->y > padding &&
        pt->y < buttonSize + padding)
    {
        return SDL_HITTEST_NORMAL;
    }

    if (pt->y < 30)
    {
        return SDL_HITTEST_DRAGGABLE;
    }

    return SDL_HITTEST_NORMAL;
}

void GUI::Dragable(bool t) {
    RunOnThreadSafe([t, this] {
        t ? SDL_SetWindowHitTest(window, HitTest, nullptr) : SDL_SetWindowHitTest(window, nullptr, nullptr);
    });
}

void GUI::ShowWindow(bool t) {
    RunOnThreadSafe([t, this] {
        t ? SDL_ShowWindow(window) : SDL_HideWindow(window);
    });
}

void GUI::Borderless(bool t) {
    RunOnThreadSafe([t, this] {
        SDL_SetWindowBordered(window, !t);
    });
}

void GUI::SetSize(int width, int height) {
    RunOnThreadSafe([width, height, this] {
        SDL_SetWindowSize(window, width, height);
    });
}

void GUI::RaiseWindow() {
    RunOnThreadSafe([this] {
        if (!window) return;
        SDL_RestoreWindow(window);
        SDL_RaiseWindow(window);
    });
}

void GUI::BringWindowToFront() {
    RunOnThreadSafe([this] {
        if (!window) return;
        SDL_RestoreWindow(window);
        SDL_RaiseWindow(window);
        SDL_HideWindow(window);
        SDL_ShowWindow(window);
    });
}

void GUI::ShowError(std::string text) {
    SDL_Delay(250);
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR,
        "DevlitchInput Error",
        text.c_str(),
        window
    );
}

void GUI::Shutdown() {
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    if (device && window) SDL_ReleaseWindowFromGPUDevice(device, window);

    if (device) {
        SDL_DestroyGPUDevice(device);
        device = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (client.func.IsConnected()) client.func.Disconnect();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);

}