// Input.h - 키보드와 마우스 (DirectXTK Keyboard, Mouse를 감싼 얇은 층)
//
// 소코반에서 중요한 것은 "키가 눌려 있는가"가 아니라 "이번 프레임에 눌린 순간인가"다.
// 눌려 있는 동안 매 프레임 이동하면 한 번 누를 때 수십 칸을 간다. 그래서 '이전 프레임 상태'와 비교한다.
//
// NEED: 09단계에서 플레이어를 한 칸씩 움직이려면 '눌린 순간'(edge) 판정이 필요해서 추가.
//       CubeViewer도 마우스 회전에 쓸 수 있으므로 Engine에 둔다.
#pragma once

#include <memory>

#include "StepApp.h"  // windows.h (Keyboard.h, Mouse.h가 UINT, WPARAM 등을 쓴다)

#include "Keyboard.h"  // DirectXTK
#include "Mouse.h"     // DirectXTK

namespace engine {

using Key = DirectX::Keyboard::Keys;

class Input {
public:
    Input();
    ~Input();

    void Attach(HWND window);  // 마우스가 좌표를 받을 창
    static void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);  // 창 메시지를 넘겨받는다

    void Update();  // 프레임마다 한 번, 게임 로직(OnFrame)보다 먼저

    bool WasPressed(Key key) const;  // 이번 프레임에 '눌린 순간'
    bool IsDown(Key key) const;      // 지금 눌려 있음
    bool IsMouseLeftDown() const;
    float MouseDeltaX() const;       // 이번 프레임에 마우스가 가로로 움직인 픽셀 (왼쪽 버튼을 누른 동안만)

private:
    std::unique_ptr<DirectX::Keyboard> keyboard_;
    std::unique_ptr<DirectX::Mouse> mouse_;
    DirectX::Keyboard::KeyboardStateTracker keys_;
    DirectX::Keyboard::State keyState_{};
    bool mouseLeft_ = false;
    int lastMouseX_ = 0;
    float mouseDeltaX_ = 0.0f;
};

}  // namespace engine
