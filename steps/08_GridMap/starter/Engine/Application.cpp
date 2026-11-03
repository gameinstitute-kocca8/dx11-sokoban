// Application.cpp
#include "Engine/Application.h"

namespace engine {

bool Application::OnInit() {
    if (!renderer_.Initialize(Window(), Width(), Height(), UseWarp())) return false;
    return OnStart();
}

void Application::OnUpdate(float deltaSeconds, float totalSeconds) {
    OnFrame(deltaSeconds, totalSeconds);
}

void Application::OnRender() {
    renderer_.BeginFrame(render::Vector4(0.10f, 0.20f, 0.35f, 1.0f));
    OnDraw(renderer_);
}

void Application::OnPresent() {
    renderer_.Present();
}

bool Application::OnCapture(const std::wstring& path) {
    return renderer_.SaveBackBufferPng(path);
}

}  // namespace engine
