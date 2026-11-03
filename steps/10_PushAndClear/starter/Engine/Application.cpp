// Application.cpp
#include "Engine/Application.h"

namespace engine {

bool Application::OnInit() {
    if (!renderer_.Initialize(Window(), Width(), Height(), UseWarp())) return false;
    input_.Attach(Window());
    return OnStart();
}

void Application::OnUpdate(float deltaSeconds, float totalSeconds) {
    input_.Update();
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

void Application::OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    Input::ProcessMessage(message, wParam, lParam);
}

}  // namespace engine
