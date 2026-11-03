// Application.cpp
#include "Engine/Application.h"

namespace engine {

bool Application::OnInit() {
    // TODO(07): Renderer를 초기화하고, 성공하면 프로그램의 OnStart를 부른다.
    //   힌트: renderer_.Initialize(Window(), Width(), Height(), UseWarp())
    return false;
}

void Application::OnUpdate(float deltaSeconds, float totalSeconds) {
    OnFrame(deltaSeconds, totalSeconds);
}

void Application::OnRender() {
    // TODO(07): 배경을 지우고(BeginFrame) 프로그램의 OnDraw를 부른다. 배경색은 01단계의 남색.
}

void Application::OnPresent() {
    renderer_.Present();
}

bool Application::OnCapture(const std::wstring& path) {
    return renderer_.SaveBackBufferPng(path);
}

}  // namespace engine
