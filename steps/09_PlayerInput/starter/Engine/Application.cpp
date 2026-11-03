// Application.cpp
#include "Engine/Application.h"

namespace engine {

bool Application::OnInit() {
    if (!renderer_.Initialize(Window(), Width(), Height(), UseWarp())) return false;
    input_.Attach(Window());
    return OnStart();
}

void Application::OnUpdate(float deltaSeconds, float totalSeconds) {
    // TODO(09): 게임 로직보다 먼저 입력 상태를 갱신한다. 순서가 바뀌면 무엇이 달라질까?
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
    // TODO(09): 창 메시지를 Input에 넘긴다. 이 줄이 없으면 키를 눌러도 Input은 아무것도 모른다.
    (void)message;
    (void)wParam;
    (void)lParam;
}

}  // namespace engine
