// Input.cpp
#include "Engine/Input.h"

namespace engine {

Input::Input() : keyboard_(std::make_unique<DirectX::Keyboard>()), mouse_(std::make_unique<DirectX::Mouse>()) {}
Input::~Input() = default;

void Input::Attach(HWND window) {
    mouse_->SetWindow(window);
}

void Input::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
    DirectX::Mouse::ProcessMessage(message, wParam, lParam);
}

void Input::Update() {
    // TODO(09): 키보드 상태를 읽어 tracker를 갱신하고, 마우스 왼쪽 버튼과 가로 이동량을 계산한다.
    //   힌트: keyState_ = keyboard_->GetState();  keys_.Update(keyState_);  ← tracker가 이전 프레임과 비교해 준다
    //   힌트: 마우스: 버튼을 누르기 시작한 프레임에는 이동량 0 (그 전 위치가 엉뚱할 수 있다)
}

bool Input::WasPressed(Key key) const {
    // TODO(09): '눌린 순간'인지 돌려준다.
    //   힌트: keys_.IsKeyPressed(key). IsDown과 무엇이 다른지 직접 실험해 보자 (TASK.md 예측 질문 1).
    (void)key;
    return false;
}

bool Input::IsDown(Key key) const {
    return keyState_.IsKeyDown(key);
}

bool Input::IsMouseLeftDown() const {
    return mouseLeft_;
}

float Input::MouseDeltaX() const {
    return mouseDeltaX_;
}

}  // namespace engine
