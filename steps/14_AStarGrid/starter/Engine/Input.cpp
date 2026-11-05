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
    keyState_ = keyboard_->GetState();
    keys_.Update(keyState_);

    const DirectX::Mouse::State mouse = mouse_->GetState();
    mouseDeltaX_ = (mouse.leftButton && mouseLeft_) ? static_cast<float>(mouse.x - lastMouseX_) : 0.0f;
    mouseLeft_ = mouse.leftButton;
    lastMouseX_ = mouse.x;
    buttons_.Update(mouse);
    mouseX_ = mouse.x;
    mouseY_ = mouse.y;
}

bool Input::WasPressed(Key key) const {
    return keys_.IsKeyPressed(key);
}

bool Input::IsDown(Key key) const {
    return keyState_.IsKeyDown(key);
}

bool Input::WasRightClicked() const {
    return buttons_.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Input::IsMouseLeftDown() const {
    return mouseLeft_;
}

float Input::MouseDeltaX() const {
    return mouseDeltaX_;
}

}  // namespace engine
