// Application.h - 창 + 루프(StepApp)와 Renderer를 이어 주는 얇은 층
//
// 01~06단계의 main.cpp는 매번 같은 일을 했다: Device 만들기, 매 프레임 지우기, Present, 캡처.
// 그 반복을 여기 한 번만 두고, 프로그램(CubeViewer, Sokoban)은 세 가지만 채운다.
//   OnStart : 메시 만들기, 레벨 불러오기
//   OnFrame : 시간에 따라 상태 바꾸기 (그리기 금지)
//   OnDraw  : 상태를 보고 그리기 (상태 바꾸기 금지)
//
// NEED: 07단계에서 CubeViewer와 Sokoban 두 프로그램이 같은 초기화/프레임 코드를 필요로 해서 추가
#pragma once

#include <string>

#include "Render/Renderer.h"
#include "StepApp.h"

namespace engine {

class Application : public step::StepApp {
public:
    using StepApp::StepApp;

protected:
    virtual bool OnStart() { return true; }
    virtual void OnFrame(float deltaSeconds, float totalSeconds) {
        (void)deltaSeconds;
        (void)totalSeconds;
    }
    virtual void OnDraw(render::Renderer& renderer) = 0;

    render::Renderer& GetRenderer() { return renderer_; }

private:
    // StepApp의 다섯 함수를 여기서 모두 채운다. 프로그램 쪽에서 다시 덮어쓰지 못하게 final.
    bool OnInit() final;
    void OnUpdate(float deltaSeconds, float totalSeconds) final;
    void OnRender() final;
    void OnPresent() final;
    bool OnCapture(const std::wstring& path) final;

    render::Renderer renderer_;
};

}  // namespace engine
